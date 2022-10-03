//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "Client.hpp"

#include <spdlog/spdlog.h>

namespace ls {

	Client::Client(SocketType<tcp> socket, std::shared_ptr<Server> server) noexcept
		: server(server),
		  socket(std::move(socket)) {
	}

	void Client::Open() noexcept {
		spdlog::info("Connection opened from {}", socket.remote_endpoint().address().to_string());

		net::co_spawn(socket.get_executor(), [self = shared_from_this()] {
			return self->ReaderCoro();
		}, net::detached);

	}

	Awaitable<void> Client::ReaderCoro() noexcept {
			char messageHeaderBuffer[sizeof(WireMessageHeader)];
			std::vector<std::uint8_t> messagePayloadBuffer;

			while(true) {
				auto [ec, n] = co_await socket.async_read_some(net::buffer(messageHeaderBuffer), use_tuple_awaitable);

				if(ec)
					break;

				if(n == 0)
					continue;

				// give up parsing this message
				if(n != sizeof(WireMessageHeader)) {
					spdlog::warn("Malformed message (read {} bytes, when header is {} bytes)", n, sizeof(WireMessageHeader));
					break;
				}

				auto header = server->reader.ReadHeader((const uint8_t*)&messageHeaderBuffer[0]);

				// Discard if the message header could not be read.
				if(!header.has_value())
					continue;

				// TODO: I dunno if the property buf is actually null terminated,
				// if it isn't we can probably remove this

				messagePayloadBuffer.resize(header->payloadSize + 1);

				auto [ payloadEc, payloadN ] = co_await socket.async_read_some(net::buffer(messagePayloadBuffer), use_tuple_awaitable);

				if(payloadN != (header->payloadSize + 1)) {
					spdlog::warn("Malformed payload (read {} bytes when we were supposed to read {} bytes)", payloadN, header->payloadSize + 1);
					break;
				}

				// Discard this message if it didn't read successfully
				if(!co_await server->reader.ReadAndHandleMessage(*header, messagePayloadBuffer, server, shared_from_this()))
					continue;

				// Add to the user's per-IP ratelimit.
			}

			Close();
	}



	void Client::Close() noexcept {
		spdlog::info("Connection closed");
		socket.close();
	}

} // namespace ls