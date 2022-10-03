//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CLIENT_HPP
#define SSX3LOBBYSERVER_CLIENT_HPP

#include <boost/asio/compose.hpp>
#include <deque>
#include <Server.hpp>
#include <MessageReader.hpp>

namespace ls {

	struct Client : public std::enable_shared_from_this<Client> {
		Client(SocketType<tcp> socket, std::shared_ptr<Server> server) noexcept;

		void Open() noexcept;

		void Close() noexcept;

		template<typename Protocol>
		struct AsyncSendImpl {
			enum class State {
				Started, ///< Write has begun.
				Written	 ///< Write has ended, either successfully or not.
			};

			SocketType<Protocol>& socket;
			std::unique_ptr<std::vector<std::uint8_t>> serializedMessage;
			State state { State::Started };

			void operator()(auto& self, const error_code& error = {}, std::size_t bytesSent = 0) {
				switch(state) {
					case State::Started:
						state = State::Written;
						socket.async_write_some(net::buffer(*serializedMessage), std::move(self));
						break;
					case State::Written:
						serializedMessage.reset();
						self.complete(error);
						break;
				}
			}
		};

		template<typename Protocol>
		AsyncSendImpl(SocketType<Protocol>, std::unique_ptr<std::vector<std::uint8_t>>&&) -> AsyncSendImpl<Protocol>;

		template <net::completion_token_for<void(error_code)> CompletionToken>
		auto AsyncSend(std::shared_ptr<MessageBase> message, CompletionToken&& token) noexcept {
			using CompletionSig = void(error_code);

			// Serialize the message into a buffer used during this operation,
			// which will be deleted once the operation either completes or fails.
			std::unique_ptr<std::vector<std::uint8_t>> serialized = std::make_unique<std::vector<std::uint8_t>>();
			message->Serialize(*serialized);

			return net::async_compose<CompletionToken, CompletionSig>(AsyncSendImpl { socket, std::move(serialized) }, token, socket);
		}

		uint32_t GetPing() const noexcept;
		void SetPing(uint32_t pingMs) noexcept;

	   private:
		Awaitable<void> ReaderCoro() noexcept;
		SocketType<tcp> socket;

		std::shared_ptr<Server> server;


		MessageReader reader;


		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_HPP
