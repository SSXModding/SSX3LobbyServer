//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_ARGUMENTS_H
#define SSX3LOBBYSERVER_ARGUMENTS_H

#include <cstdint>
#include <string>

namespace ls {
	/**
	 * Command line arguments to the server.
	 */
	struct Arguments {
		void Process(int argc, char** argv);

		const std::string& GetListenAddress() const;
		//uint16_t Get

	   private:
		std::string listenAddress;
		//uint16_t port;
		//uint16_t rdirport;
	};
} // namespace ls

#endif // SSX3LOBBYSERVER_ARGUMENTS_H
