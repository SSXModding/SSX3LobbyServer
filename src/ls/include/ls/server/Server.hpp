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

#include <ls/asio/AsioConfig.hpp>

namespace ls {

	struct PerIPData {
		/**
		 * Number of connections from this IP
		 */
		std::uint16_t nrConnections;

		/**
		 * Message rate limit for this IP..
		 */
		//RateLimit<> messageRateLimit { 15 /* messages */, 1 /* second */, 5 /* seconds cooldown */ };
	};

	struct Server : public std::enable_shared_from_this<Server> {
		void Start();

		void Stop(); // TODO: for clean shutdowns

	   private:
		friend struct Client;

		Awaitable<void> ListenerCoro(AcceptorType<tcp> acceptor);

		// TODO: maybe timer for stuff or keep alive
		// if ~png is sent that much

		// TODO: hold the DB here

		// MAJOR TODO: Implement graceful cancellation.
		// 		       on SIGTERM we should close all connections, the DB, and then exit 0

		ExecutorType executor = MakeExecutor();

		ExecutorType MakeExecutor() noexcept {
			return net::make_strand(BaseExecutorType{});
		}
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_SERVER_HPP
