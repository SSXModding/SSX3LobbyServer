//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include <ls/config/backends/CommandLineConfigBackend.hpp>
#include <ls/config/ConfigStore.hpp>
#include <ls/server/Server.hpp>

/**
 * The server-wide global instance of the config store.
 */
ls::ConfigStore gConfigStore;

int main(int argc, char** argv) {
	// Initialize and run all configuration backends.
	// priority pretty much will be last backend to set the value, where:
	//
	//  - cli (so we can choose an alternative config file)
	// - toml (if an option wasnt set by cli)
	// - defaults

	ls::CommandLineConfigBackend clibackend(gConfigStore);

	clibackend.Process(argc, argv);

	// TODO: Maybe context thread pool.
	// 	(see asio/thread_pool.hpp)

	// Boot the server up.
	ls::Server server;
	server.Start();

	// The main thread doesn't do anything when using the system_executor, so
	// we just have it wait for the executor to run out of work.
	ls::net::query(ls::BaseExecutorType{}, ls::net::execution::context).join();
	return 0;
}
