#include <BaseMessage.h>
#include <MessageReader.h>

#include <fourcc.h>

#include <Server.h>

#include <iostream>
#include "Arguments.h"

int main(int argc, char** argv) {
	ls::Arguments a;
	a.Process(argc, argv);

	// Initialize messages before doing anything with the server.
	// We could use static initializers, but they seem to be finicky when
	// using static libs on different platforms.
	ls::InitMessages();

#if 1
	boost::asio::io_context ioc(1);

	auto server = std::make_shared<ls::Server>(ioc);
	server->Start();

	ioc.run();
#endif

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
	std::cout.write((const char*)&buf[0], buf.size());
#endif
	return 0;
}
