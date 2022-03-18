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


// HACK: The ASIO awaitable header needs this to force CLion
// into seeing awaitable<T>. Clangd seems to pick it up
// fine but clion's internal engine seems to not make the
// ASIO features header turn on co_await support, which
// breaks stuff. :(.
//
// TODO: File a bug with jetbrains? I may actually do so
// either that or wait until it hopefully isn't needed anymore.
#ifdef __CLION_IDE_
	#define GENERATING_DOCUMENTATION
#endif

#include <boost/asio/awaitable.hpp>

#ifdef __CLION_IDE_
	#undef GENERATING_DOCUMENTATION
#endif

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace ls {

	namespace asio = boost::asio;
	namespace ip = asio::ip;
	using tcp = ip::tcp;

	struct Server : public std::enable_shared_from_this<Server> {
		Server(asio::io_context& ioc);

		void Start();

	   private:
		asio::io_context& ioc;


		asio::awaitable<void> listener();

		// TODO: maybe timer for stuff or keep alive
		// if ~png is sent that much

		// TODO: hold the DB here


		MessageReader reader;

	};

} // namespace ls

#endif // SSX3LOBBYSERVER_SERVER_H
