//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include "BaseMessage.h"
#include "Client.h"

constexpr static auto TYPE_CODE = ls::FourCCValue("test");

struct PingMessage : public ls::MessageBase {
	PingMessage()
		: MessageBase() {
		typeCode = TYPE_CODE;
	}

	void CreateDefaultProperties() override {
		properties["TIME"] = "";
	}

	void HandleMessage(std::shared_ptr<ls::Server> server, std::shared_ptr<ls::Client> client) override {
		// client->Ping = ping calc here...

		spdlog::info("PingMessage::HandleMessage()");
	}
};

void InitPing() {
	LSRegisterMessage(TYPE_CODE, PingMessage);
}