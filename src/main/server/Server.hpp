//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_SERVER_HPP
#define SSX3LOBBYSERVER_SERVER_HPP

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

#include <asio/AsioConfig.hpp>
#include "RateLimit.hpp"

namespace ls {

	struct PerIPData {

		/**
		 * Number of connections from this IP
		 */
		std::uint16_t nrConnections;

		/**
		 * Message rate limit for this IP..
		 */
		RateLimit<> messageRateLimit{ 15 /* messages */, 1 /* second */, 5 /* seconds cooldown */ };


	};

	struct Server : public std::enable_shared_from_this<Server> {
		explicit Server(net::io_context& ioc) noexcept;

		void Start() noexcept;

		void Stop() noexcept; // TODO: for clean shutdowns

	   private:
		friend struct Client;
		net::io_context& ioc;


		Awaitable<void> ListenerCoro(AcceptorType<tcp> acceptor) noexcept;

		// TODO: maybe timer for stuff or keep alive
		// if ~png is sent that much

		// TODO: hold the DB here


		ExecutorType MakeExecutor() noexcept {
			return net::make_strand(
#ifdef LS_USE_SYSTEM_EXECUTOR
			// This is ugly but if we're here we're using system_executor.
			net::system_executor {}
#else
			ioc.get_executor()
#endif
			);
		}

	};

} // namespace ls

#endif // SSX3LOBBYSERVER_SERVER_HPP
