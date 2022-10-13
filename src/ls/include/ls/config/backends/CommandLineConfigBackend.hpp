//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_COMMANDLINECONFIGBACKEND_HPP
#define SSX3LOBBYSERVER_COMMANDLINECONFIGBACKEND_HPP

#include <cstdint>
#include <string>

#include <ls/config/ConfigStore.hpp>

namespace ls {
	/**
	 * Command line arguments to the server.
	 */
	struct CommandLineConfigBackend {
		explicit CommandLineConfigBackend(ConfigStore& store);

		void Process(int argc, char** argv);

	   private:
		ConfigStore& configStore;
	};
} // namespace ls

#endif // SSX3LOBBYSERVER_COMMANDLINECONFIGBACKEND_HPP
