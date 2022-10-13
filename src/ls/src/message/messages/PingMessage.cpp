//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <spdlog/spdlog.h>

#include <ls/server/Client.hpp>
#include <ls/server/message/MessageBase.hpp>

struct PingMessage : public ls::Message<"~png", PingMessage> {
	explicit PingMessage() {
	}

	ls::Awaitable<void> HandleClientMessage(std::shared_ptr<ls::Server> server, std::shared_ptr<ls::Client> client) override {
		co_return;
	}
};