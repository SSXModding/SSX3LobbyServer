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

#include <boost/asio/steady_timer.hpp>
#include <deque>
#include <Server.hpp>

namespace ls {

	struct Client : public std::enable_shared_from_this<Client> {
		Client(SocketType<tcp> socket, std::shared_ptr<Server> server) noexcept;

		void Open() noexcept;

		void Close() noexcept;

		// maybe rethink design of sending messages and have a
		// AsyncSend(shared_ptr<MessageBase>, CompletionToken)
		// which queues up and then calls token when our message is written

		/**
		 * Add a message to the writer queue.
		 */
		void AddMessage(std::shared_ptr<MessageBase> message) noexcept;


		uint32_t GetPing() const noexcept;
		void SetPing(uint32_t pingMs) noexcept;

	   private:
		Awaitable<void> ReaderCoro() noexcept;
		Awaitable<void> WriterCoro() noexcept;

		SocketType<tcp> socket;

		TimerType messageWriteTimer;
		std::deque<std::shared_ptr<MessageBase>> messageWriteQueue;

		std::shared_ptr<Server> server;

		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_H
