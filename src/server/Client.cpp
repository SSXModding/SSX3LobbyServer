//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "Client.h"

#include <spdlog/spdlog.h>

namespace ls {

	Client::Client(tcp::socket socket, std::shared_ptr<Server> server)
		: messageWriteTimer(socket.get_executor()),
		  server(server),
		  socket(std::move(socket)) {
		messageWriteTimer.expires_after(std::chrono::nanoseconds(500));
	}

	void Client::Open() {
		spdlog::info("Connection opened from {}", socket.remote_endpoint().address().to_string());

		asio::co_spawn(
		socket.get_executor(), [self = shared_from_this()] {
			return self->ReaderCoro(); // co_return?
		},
		asio::detached);

		asio::co_spawn(
		socket.get_executor(), [self = shared_from_this()] {
			return self->WriterCoro(); // co_return?
		},
		asio::detached);
	}

	void Client::AddMessage(std::shared_ptr<MessageBase> message) {
		messageWriteQueue.push_back(message);
		messageWriteTimer.cancel_one();
	}

	asio::awaitable<void> Client::ReaderCoro() {
		try {
			std::vector<std::uint8_t> messagePayloadBuffer;

			while(true) {
				char header_buffer[sizeof(WireMessageHeader)];
				auto n = co_await socket.async_read_some(asio::buffer(header_buffer), asio::use_awaitable);

				if(n == 0)
					continue;

				// give up parsing this message
				if(n != sizeof(WireMessageHeader)) {
					spdlog::warn("Malformed message (read {} bytes)", n);
					continue;
				}

				auto header = server->reader.ReadHeader((const uint8_t*)&header_buffer[0]);
				if(!header.has_value())
					continue;

				// TODO: I dunno if the property buf is actually null terminated,
				// if it isn't we can probably remove this

				messagePayloadBuffer.resize(header->payloadSize + 1);

				n = co_await socket.async_read_some(asio::buffer(messagePayloadBuffer), asio::use_awaitable);

				if(n != (header->payloadSize + 1)) {
					spdlog::warn("Malformed payload (read {} bytes)", n);
					continue;
				}

				server->reader.ReadAndHandleMessage(*header, messagePayloadBuffer, server, shared_from_this());
			}

		} catch(std::exception& ex) {
			Close();
		}
	}

	asio::awaitable<void> Client::WriterCoro() {
		try {
			while(socket.is_open()) {
				if(messageWriteQueue.empty()) {
					boost::system::error_code ec;
					co_await messageWriteTimer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
				} else {
					std::vector<std::uint8_t> serializationBuf;
					auto message = messageWriteQueue.front();

					message->Serialize(serializationBuf);

					co_await socket.async_write_some(asio::buffer(serializationBuf), asio::use_awaitable);

					messageWriteQueue.pop_front();
				}
			}
		} catch(std::exception& ex) {
			Close();
		}
	}

	void Client::Close() {
		spdlog::info("Connection closed");
		socket.close();
		messageWriteQueue.clear(); // TODO: This statement should probably be part of the destructor
		messageWriteTimer.cancel();
	}

} // namespace ls