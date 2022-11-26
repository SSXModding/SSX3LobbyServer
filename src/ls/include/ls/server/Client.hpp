//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CLIENT_HPP
#define SSX3LOBBYSERVER_CLIENT_HPP

#include <ls/asio/AsioConfig.hpp>

namespace ls {
	struct Server;

	struct Client : public std::enable_shared_from_this<Client> {
		Client(SocketType<tcp> socket, Server& server);

		/**
		 * Called to open the connection.
		 */
		void Open();

		/**
		 * Called to close the connection.
		 */
		void Close();

		net::ip::address Address() {
			return socket.remote_endpoint().address();
		}

		Server& GetServer() {
			return server;
		}

	   private:
		Awaitable<void> ReaderCoro();
		SocketType<tcp> socket;

		Server& server;

		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_HPP
