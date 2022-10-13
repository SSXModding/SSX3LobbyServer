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
#include <ls/config/ConfigStore.hpp>
#include <ls/server/Client.hpp>
#include <ls/server/Server.hpp>

extern ls::ConfigStore gConfigStore;

namespace ls {

	constexpr auto BUDDY_PORT = 10998;

	// Game connects to ps2ssx04.ea.com:11000
	constexpr auto GAME_PORT = 11000;

	Server::Server(net::io_context& ioc)
		: ioc(ioc) {
	}

	void Server::Start() {
		net::ip::address address;

		if(auto listen_address = gConfigStore.GetValue<std::string>("listen_address"); listen_address.has_value()) {
			address = net::ip::make_address(listen_address.value());
		} else {
			// Ok, then we can just use 0.0.0.0
			// FIXME: should have a default set in the config store, rather than hardcoding it here.
			// We can remove this branch after that (and if somehow we do get here assert or something?)
			address = net::ip::make_address("0.0.0.0");
		}

		// Spawn a server on the game port
		net::co_spawn(
		executor, [self = shared_from_this(), address]() {
			return self->ListenerCoro(AcceptorType<tcp> { self->executor, tcp::endpoint { address, GAME_PORT }, true });
		},
		net::detached);

		// TODO: Buddy port, maybe a UDP thingy if required
	}

	Awaitable<void> Server::ListenerCoro(AcceptorType<tcp> acceptor) {
		auto listen_ep = acceptor.local_endpoint();
		spdlog::info("Listening on {}:{}", listen_ep.address().to_string(), listen_ep.port());

		for(;;) {
			auto [ec, socket] = co_await acceptor.async_accept(use_tuple_awaitable);

			// should probably draw from context pool/make a new executor for this connection,
			// but for now this is fine
			if(!ec)
				std::make_shared<Client>(std::move(socket), shared_from_this())->Open();
			else
				continue; // Cancellation will need this to break on cancels.
		}
	}

} // namespace ls