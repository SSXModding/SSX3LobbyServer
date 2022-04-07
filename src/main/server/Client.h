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

#include <boost/asio/steady_timer.hpp>
#include <deque>

namespace ls {

	struct Client : public std::enable_shared_from_this<Client> {
		Client(tcp::socket socket, std::shared_ptr<Server> server);

		void Open();

		void Close();

		/**
		 * Add a message to the writer queue.
		 */
		void AddMessage(std::shared_ptr<MessageBase> message);


		uint32_t GetPing() const;
		void SetPing(uint32_t pingMs);

	   private:
		asio::awaitable<void> ReaderCoro();
		asio::awaitable<void> WriterCoro();

		tcp::socket socket;

		asio::steady_timer messageWriteTimer;
		std::deque<std::shared_ptr<MessageBase>> messageWriteQueue;

		std::shared_ptr<Server> server;

		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_H
