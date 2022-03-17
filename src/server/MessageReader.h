//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_MESSAGEREADER_H
#define SSX3LOBBYSERVER_MESSAGEREADER_H

#include <BaseMessage.h>

namespace ls {

	struct MessageReader {

		void ReadMessage(const std::vector<std::uint8_t>& buf, std::shared_ptr<Server> server, std::shared_ptr<Client> client);

	};

}

#endif // SSX3LOBBYSERVER_MESSAGEREADER_H
