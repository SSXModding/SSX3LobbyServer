//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include <ls/server/Client.hpp>

namespace ls {



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
			spdlog::info("[{}] Connection opened", Address().to_string());

			while(true) {
				auto [ readError, messagePtr ] = co_await AsyncReadMessage(use_tuple_awaitable);

				if(!readError) {
					if(readError.type == ReadError::Type::AsioError)
						spdlog::warn("[{}] Asio read error: {}", Address().to_string(), readError.asioError.message());
					else {
						spdlog::warn("[{}] Read error: {}", Address().to_string(), readError.what);
					}

					break;
				} else {
					co_await messagePtr->HandleClientMessage(shared_from_this());
				}
			}

			Close();
	}


	void Client::Close() {
		spdlog::info("[{}] Connection closed", Address().to_string());
		socket.close();
	}

} // namespace ls