//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_SERVER_H
#define SSX3LOBBYSERVER_SERVER_H

#include <MessageReader.h>


// CLion + C++20 coros don't mix well for some reason

#ifdef __CLION_IDE_
#define BOOST_ASIO_HAS_CO_AWAIT
#define BOOST_ASIO_HAS_STD_COROUTINE
#endif

#include <boost/asio/awaitable.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/write.hpp>

namespace ls {

	namespace asio = boost::asio;
	namespace ip = asio::ip;
	using tcp = ip::tcp;

	struct Server : public std::enable_shared_from_this<Server> {
		explicit Server(asio::io_context& ioc);

		void Start();

		void SendMessage(std::shared_ptr<MessageBase> message);

	   private:
		friend struct Client;
		asio::io_context& ioc;


		asio::awaitable<void> ListenerCoro(tcp::acceptor acceptor);

		// TODO: maybe timer for stuff or keep alive
		// if ~png is sent that much

		// TODO: hold the DB here


		MessageReader reader;

	};

} // namespace ls

#endif // SSX3LOBBYSERVER_SERVER_H
