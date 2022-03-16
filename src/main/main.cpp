#include <BaseMessage.h>
#include <MessageReader.h>

#include <fourcc.h>

#include <iostream>

int main() {

	std::vector<std::uint8_t> buf;

	auto msg = ls::CreateMessageFromTypeCode(ls::FourCCValue("test"));

	// Set test properties
	msg->GetProperty("test") = "hi";
	msg->GetProperty("test2") = "I'm water";
	msg->GetProperty("test3") = "\"Water has left the game.\"";
	msg->GetProperty("test4") = "nested = test this should not terminate or break the message";

	msg->Serialize(buf);

	printf("DEBUG: buf len is %lu\n", buf.size());
	//std::cout.write((const char*)&buf[0], buf.size());

	// new MessageReader API test.
	ls::MessageReader reader;
	reader.ReadMessage(buf, nullptr);
	return 0;
}
