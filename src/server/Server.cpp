//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "Server.h"

#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>

namespace ls {

	Server::Server(asio::io_context& ioc)
		: ioc(ioc) {

	}

	void Server::Start() {
		// start listener(s) on specific ports
	}


	asio::awaitable<void> Server::listener() {

	}

}