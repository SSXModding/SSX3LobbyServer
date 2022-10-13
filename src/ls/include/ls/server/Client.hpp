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

#include <spdlog/spdlog.h>

#include <boost/asio/compose.hpp>
#include <ls/common/Badge.hpp>
#include <ls/server/message/MessageBase.hpp>
#include <ls/server/Server.hpp>

namespace ls {

	struct Client : public std::enable_shared_from_this<Client> {
		Client(SocketType<tcp> socket, std::shared_ptr<Server> server);

		void Open();

		void Close();

		template <typename Protocol>
		struct AsyncSendImpl {
			enum class State {
				Started, ///< Write has begun.
				Written	 ///< Write has ended, either successfully or not.
			};

			explicit constexpr AsyncSendImpl(SocketType<Protocol>& socket, std::vector<std::uint8_t>&& message)
				: socket(socket),
				  serializedMessage(std::move(message)) {
			}

			SocketType<Protocol>& socket;
			std::vector<std::uint8_t> serializedMessage;
			State state { State::Started };

			void operator()(auto& self, const error_code& error = {}, std::size_t bytesSent = 0) {
				switch(state) {
					case State::Started:
						state = State::Written;
						socket.async_write_some(net::buffer(serializedMessage), std::move(self));
						break;
					case State::Written:
						self.complete(error);
						break;
				}
			}
		};

		template <typename Protocol>
		AsyncSendImpl(SocketType<Protocol>, std::unique_ptr<std::vector<std::uint8_t>>&&) -> AsyncSendImpl<Protocol>;

		template <net::completion_token_for<void(error_code)> CompletionToken>
		auto AsyncSend(std::shared_ptr<MessageBase> message, CompletionToken&& token) {
			using CompletionSig = void(error_code);

			// Serialize the message into a buffer used during this operation,
			// which will be cleared once the operation either completes or fails.
			std::vector<std::uint8_t> serialized;
			message->Serialize(serialized);

			return net::async_compose<CompletionToken, CompletionSig>(AsyncSendImpl { socket, std::move(serialized) }, token, socket);
		}

		net::ip::address Address() {
			return socket.remote_endpoint().address();
		}

        std::shared_ptr<Server> GetServer() {
            return server;
        }

	   private:

		Awaitable<void> ReaderCoro();
		SocketType<tcp> socket;

		std::shared_ptr<Server> server;

		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_HPP
