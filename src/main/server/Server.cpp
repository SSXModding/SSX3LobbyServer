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

#include <config/ConfigStore.h>

#include <spdlog/spdlog.h>


extern ls::ConfigStore gConfigStore;


namespace ls {

	constexpr auto BUDDY_PORT = 10998;

	// Game connects to ps2ssx04.ea.com:11000
	constexpr auto GAME_PORT = 11000;

	Server::Server(asio::io_context& ioc)
		: ioc(ioc) {
	}

	void Server::Start() {
		asio::ip::address address;

		auto listen_address = gConfigStore.GetValue<std::string>("listen_address");

		if(listen_address.has_value()) {
			address = asio::ip::make_address(listen_address.value());
		} else {
			// Ok, then we can just use 0.0.0.0
			// FIXME: should have a default set in the config store, rather than hardcoding it here.
			// We can remove this branch after that (and if somehow we do get here assert or something?)
			address = asio::ip::make_address("0.0.0.0");
		}

		// Spawn a server on the game port
		asio::co_spawn(ioc, shared_from_this()->ListenerCoro(tcp::acceptor { ioc, tcp::endpoint { address, GAME_PORT }, true }), asio::detached);
		// TODO: Buddy port, maybe a UDP thingy if required
	}

	asio::awaitable<void> Server::ListenerCoro(tcp::acceptor acceptor) {
		// TODO: catch exceptions (so we can keep going gracefully)

		auto listen_ep = acceptor.local_endpoint();
		spdlog::info("Listening on {}:{}", listen_ep.address().to_string(), listen_ep.port());

		for(;;) {
			std::make_shared<Client>(co_await acceptor.async_accept(asio::use_awaitable), shared_from_this())->Open();
		}
	}

} // namespace ls