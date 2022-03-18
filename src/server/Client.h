//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CLIENT_H
#define SSX3LOBBYSERVER_CLIENT_H

#include <Server.h>
#include <deque>

#include <boost/asio/steady_timer.hpp>

namespace ls {

struct Client {

   private:

	asio::awaitable<void> ReaderCoro();
	asio::awaitable<void> WriterCoro();

	std::deque<std::shared_ptr<MessageBase>> messageWriteQueue;
};

}

#endif // SSX3LOBBYSERVER_CLIENT_H
