//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

// Includes for debug code.
#ifndef NDEBUG
	#include <boost/core/demangle.hpp>
#endif

#include <config/backends/CommandLineConfigBackend.h>
#include <config/ConfigStore.h>
#include <Server.h>
#include <spdlog/spdlog.h>

// Includes for test code
#ifdef TEST
	#include <MessageBase.h>
	#include <MessageReader.h>
	#include <fourcc.h>
#endif

/**
 * The server-wide global instance of the config store.
 */
ls::ConfigStore gConfigStore;

#ifdef TEST
bool SerializeTest() {
	std::vector<std::uint8_t> buf;

	auto msg = ls::CreateMessageFromTypeCode(ls::FourCCValue("test"));

	// Set test properties
	msg->GetProperty("test") = "hi";
	msg->GetProperty("test2") = "I'm water";
	msg->GetProperty("test3") = "\"Water has left the game.\"";
	msg->GetProperty("test4") = "nested = test this should not terminate or break the message";

	msg->Serialize(buf);

	// printf("DEBUG: buf len is %lu\n", buf.size());
	// std::cout.write((const char*)&buf[0], buf.size());

	ls::MessageReader reader;
	auto h = reader.ReadHeader(&buf[0]);
	if(!h.has_value()) {
		std::cout << "header parse failed\n";
		return false;
	} else {
		if(!reader.ReadAndHandleMessage(*h, { buf.begin() + sizeof(ls::WireMessageHeader), buf.end() }, nullptr, nullptr)) {
			std::cout << "reader parse failed. :(\n";
			return false;
		}
		return true;
	}
}
#endif

int main(int argc, char** argv) {
	// Initialize and run all configuration backends.
	// priority pretty much will be last backend to set the value, where:
	//  - cli (so we can choose an alternative config file)
	// - toml (if an option wasnt set by cli)

	ls::CommandLineConfigBackend clibackend(gConfigStore);

	clibackend.Process(argc, argv);

	// Garbage testcase for (de)serialization
#ifdef TEST // TODO: move to catch2 tests
	return (SerializeTest() == true);
#endif

	boost::asio::io_context ioc(1);

	auto server = std::make_shared<ls::Server>(ioc);

	try {
		server->Start();
		ioc.run();
	} catch(std::exception& ex) {
#ifndef NDEBUG
		spdlog::error("Uncaught exception (of type {}): {}", boost::core::demangle(typeid(ex).name()), ex.what());
#else
		spdlog::error("Uncaught exception: {}", ex.what());
#endif

		// shut everything down
		if(!ioc.stopped())
			ioc.stop();

		return 1;
	}
	return 0;
}
