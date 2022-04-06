//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <MessageBase.h>
#include <MessageReader.h>

#include <fourcc.h>

#include <Server.h>

#include <iostream>

#include <config/ConfigStore.h>
#include <config/backends/CommandLineConfigBackend.h>

/**
 * The server-wide global instance of the config store.
 */
ls::ConfigStore gConfigStore;

int main(int argc, char** argv) {
	// other backends. priority pretty much will be last backend to set the value
	ls::CommandLineConfigBackend clibackend(gConfigStore);

	clibackend.Process(argc, argv);


	// Garbage testcase for (de)serialization
#if 0 // TODO: move to catch2 tests
	std::vector<std::uint8_t> buf;

	auto msg = ls::CreateMessageFromTypeCode(ls::FourCCValue("test"));

	// Set test properties
	msg->GetProperty("test") = "hi";
	msg->GetProperty("test2") = "I'm water";
	msg->GetProperty("test3") = "\"Water has left the game.\"";
	msg->GetProperty("test4") = "nested = test this should not terminate or break the message";

	msg->Serialize(buf);

	//printf("DEBUG: buf len is %lu\n", buf.size());
	//std::cout.write((const char*)&buf[0], buf.size());

	ls::MessageReader reader;
	auto h = reader.ReadHeader(&buf[0]);
	if(!h.has_value()) {
		std::cout << "fuck\n";
	} else {
		reader.ReadAndHandleMessage(*h, {buf.begin() + sizeof(ls::WireMessageHeader), buf.end()}, nullptr, nullptr);
	}

#endif

	boost::asio::io_context ioc(1);

	auto server = std::make_shared<ls::Server>(ioc);
	server->Start();

	ioc.run();

	return 0;
}
