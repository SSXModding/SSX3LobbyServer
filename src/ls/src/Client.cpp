//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <ls/server/Client.hpp>
#include <ls/server/message/WireMessageHeader.hpp>

#include <spdlog/spdlog.h>

namespace ls {

	// 1MB max payload size; should be good enough?
	constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024);


	Client::Client(SocketType<tcp> socket, std::shared_ptr<Server> server)
		: server(server),
		  socket(std::move(socket)) {

	}

	void Client::Open() {
		net::co_spawn(socket.get_executor(), [self = shared_from_this()]() {
			return self->ReaderCoro();
		}, net::detached);
	}

	Awaitable<void> Client::ReaderCoro() {
			char messageHeaderBuffer[sizeof(WireMessageHeader)];
			WireMessageHeader messageHeader;
			std::vector<std::uint8_t> messagePayloadBuffer;

			// Reserve a decent amount of bytes upfront so that
			// (most) message reads won't cause a memory (re)allocation.
			//
			// Bigger messages probably will; however the loop does not destroy the vector,
			// so the memory can still be reused.
			messagePayloadBuffer.reserve(512);

			spdlog::info("[{}] Connection opened", Address().to_string());


			while(true) {
				auto [ec, n] = co_await socket.async_read_some(net::buffer(messageHeaderBuffer), use_tuple_awaitable);

				if(n == 0)
					continue;

				if(ec && ec != net::error::operation_aborted) {
					spdlog::error("[{}] ASIO error: {}", Address().to_string(), ec.message());
					break;
				}

				if(n != sizeof(WireMessageHeader)) {
					spdlog::warn("[{}] Malformed message header (read {} bytes, when header is {} bytes)", Address().to_string(), n, sizeof(WireMessageHeader));
					break;
				}

				memcpy(&messageHeader, &messageHeaderBuffer[0], sizeof(WireMessageHeader));

				if(messageHeader.payloadSize > MAX_PAYLOAD_SIZE) {
					spdlog::warn("[{}] Malformed message went beyond maximum payload size ({}/{}); closing connection", Address().to_string(), MAX_PAYLOAD_SIZE, messageHeader.payloadSize);
					break;
				}

				// TODO: I dunno if the property buf is actually null terminated,
				// if it isn't we can probably remove addition

				messagePayloadBuffer.resize(messageHeader.payloadSize + 1);

				auto [ payloadEc, payloadN ] = co_await socket.async_read_some(net::buffer(messagePayloadBuffer), use_tuple_awaitable);

				if(payloadN != (messageHeader.payloadSize + 1)) {
					spdlog::warn("[{}] Malformed payload (read {} bytes when we were supposed to read {} bytes)", Address().to_string(), payloadN, messageHeader.payloadSize + 1);
					break;
				}

				auto message = ls::MessageBase::Create(messageHeader.typeCode);

				// Just handle the message if there's no property data.
				if(messageHeader.payloadSize == 1 || messageHeader.payloadSize == 0) {
					co_await message->HandleClientMessage(shared_from_this());
				} else {
					// Read in the property buffer.
					// Fail if this fails
					if(!message->ReadProperties(messagePayloadBuffer)) {
						spdlog::warn("[{}] Failed to parse property buffer", Address().to_string());
						break;
					}

					co_await message->HandleClientMessage(shared_from_this());
				}
			}

			Close();
	}


	void Client::Close() {
		spdlog::info("[{}] Connection closed", Address().to_string());
		socket.close();
	}

} // namespace ls