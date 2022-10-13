//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <ls/server/Client.hpp>

#include <spdlog/spdlog.h>

namespace ls {

	// 1MB max payload size; should be good enough?
	constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024);


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
			WireMessageHeader messageHeader;
			std::vector<std::uint8_t> messagePayloadBuffer;

			// Reserve a decent amount of bytes upfront so that
			// (most) message reads won't cause a memory (re)allocation.
			//
			// Bigger messages probably will; however the loop does not destroy the vector,
			// so the memory can still be reused.
			messagePayloadBuffer.reserve(512);

			while(true) {
				auto [ec, n] = co_await socket.async_read_some(net::buffer(messageHeaderBuffer), use_tuple_awaitable);

				if(n == 0)
					continue;

				if(ec)
					break;

				if(n != sizeof(WireMessageHeader)) {
					spdlog::warn("Malformed message (read {} bytes, when header is {} bytes)", n, sizeof(WireMessageHeader));
					break;
				}

				memcpy(&messageHeader, &messageHeaderBuffer[0], sizeof(WireMessageHeader));

				if(messageHeader.payloadSize > MAX_PAYLOAD_SIZE) {
					spdlog::warn("Malformed message went beyond maximum payload size ({}/{}); closing connection", MAX_PAYLOAD_SIZE, messageHeader.payloadSize);
					break;
				}

				// TODO: I dunno if the property buf is actually null terminated,
				// if it isn't we can probably remove this

				messagePayloadBuffer.resize(messageHeader.payloadSize + 1);

				auto [ payloadEc, payloadN ] = co_await socket.async_read_some(net::buffer(messagePayloadBuffer), use_tuple_awaitable);

				if(payloadN != (messageHeader.payloadSize + 1)) {
					spdlog::warn("Malformed payload (read {} bytes when we were supposed to read {} bytes)", payloadN, messageHeader.payloadSize + 1);
					break;
				}

				// Discard this message if it didn't read successfully
				if(!co_await reader.ReadAndHandleMessage(messageHeader, messagePayloadBuffer, server, shared_from_this()))
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