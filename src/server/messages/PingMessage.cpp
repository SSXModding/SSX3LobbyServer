//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "BaseMessage.h"
// fully defined client.

namespace ls {

	constexpr static auto TYPE_CODE = ls::FourCCValue("~png");

	struct PingMessage : public MessageBase {
		PingMessage() : MessageBase() {
			typeCode = TYPE_CODE;
		}

		void CreateDefaultProperties() override {
			properties["TIME"] = "";
		}

		void HandleMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) override {
			// in debug probably assert ~png typecode???

			//client->Ping = ping calc here...
		}
	};

	LSRegisterMessage(TYPE_CODE, PingMessage);
}