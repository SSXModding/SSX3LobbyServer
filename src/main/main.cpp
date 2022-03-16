#include <BaseMessage.h>
#include <fourcc.h>

#include <iostream>

int main() {

	std::vector<std::uint8_t> buf;

	auto msg = ls::CreateMessageFromTypeCode(ls::FourCCValue("test"));

	auto msg2 = ls::CreateMessageFromTypeCode(ls::FourCCValue("test"));

	msg->GetProperty("test") = "hi";
	msg->GetProperty("test2") = "I'm water";

	msg->GetProperty("test3") = "\"Water has left the game.\"";

	msg->GetProperty("test4") = "nested = test this should not terminate or break the message";

	msg->Serialize(buf);

	printf("buf len is %lu\n", buf.size());

	//std::cout.write((const char*)&buf[0], buf.size());

	// try to read into another message
	msg2->Read(buf, nullptr);
	return 0;
}
