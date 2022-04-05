//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "Server.h"
#include "Client.h"

#include <spdlog/spdlog.h>


namespace ls {

	constexpr auto BUDDY_PORT = 10998;

	// Game connects to ps2ssx04.ea.com:11000
	constexpr auto GAME_PORT = 11000;

	Server::Server(asio::io_context& ioc)
		: ioc(ioc) {
	}

	void Server::Start() {
		spdlog::info("Server started");
		asio::co_spawn(ioc, shared_from_this()->ListenerCoro(tcp::acceptor { ioc, tcp::endpoint { tcp::v4(), GAME_PORT }, true }), asio::detached);
	}

	asio::awaitable<void> Server::ListenerCoro(tcp::acceptor acceptor) {
		// TODO: catch exceptions (so we can keep going gracefully)
		for(;;) {
			std::make_shared<Client>(co_await acceptor.async_accept(asio::use_awaitable), shared_from_this())->Open();
		}
	}

} // namespace ls