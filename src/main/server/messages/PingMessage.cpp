//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include "Client.hpp"
#include "MessageBase.hpp"

constexpr static auto TYPE_CODE = ls::FourCCValue("~png");

struct PingMessage : public ls::MessageBase {
	PingMessage()
		: MessageBase() {
		typeCode = TYPE_CODE;
	}

	void CreateDefaultProperties() override {
		properties["TIME"] = "";
	}

	ls::Awaitable<void> HandleClientMessage(std::shared_ptr<ls::Server> server, std::shared_ptr<ls::Client> client) override {
		co_return;
	}
};

LSRegisterMessage(TYPE_CODE, PingMessage);