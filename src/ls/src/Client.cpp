//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <ls/server/Client.hpp>
#include <ls/server/message/MessageIo.hpp>
#include <ls/server/Server.hpp>

namespace ls {

	Client::Client(SocketType<tcp> socket, Server& server)
		: server(server),
		  socket(std::move(socket)) {
	}

	void Client::Open() {
		// clang-format off
		net::co_spawn(socket.get_executor(), [self = shared_from_this()]() {
			return self->ReaderCoro();
		}, net::detached);
		// clang-format on
	}

	Awaitable<void> Client::ReaderCoro() {
		spdlog::info("[{}] Connection opened", Address().to_string());

		while(true) {
			auto [ec, messagePtr] = co_await AsyncReadMessage(socket, use_tuple_awaitable);

			if(ec) {
				spdlog::warn("[{}] FUCK {}",Address().to_string(), ec.message());
				if(ec != net::error::operation_aborted && ec != net::error::eof)
					spdlog::warn("[{}] Asio read error: {}", Address().to_string(), ec.message());
				else
					if(ec == net::error::operation_aborted)
						spdlog::warn("[{}] Client read timeout, closing connection", Address().to_string());
				break;
			}

			co_await messagePtr->HandleClientMessage(shared_from_this());
		}

		Close();
	}

	void Client::Close() {
		spdlog::info("[{}] Connection closed", Address().to_string());
		socket.close();
	}

} // namespace ls