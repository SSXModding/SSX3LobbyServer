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

	struct PingMessage : public MessageBase {
		void HandleMessage(std::shared_ptr<Client> client) override {
			//client->Ping = ping calc here...
		}
	};

	REGISTER_MESSAGE('~png', PingMessage);
}