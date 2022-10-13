//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <ls/server/Client.hpp>
#include <ls/server/message/MessageReader.hpp>

namespace ls {

	Awaitable<bool> MessageReader::ReadAndHandleMessage(const WireMessageHeader& header, const std::vector<std::uint8_t>& buf, std::shared_ptr<Server> server, std::shared_ptr<Client> client) noexcept {
		// Create the message instance from the message factory
		auto message = ls::MessageBase::Create(header.typeCode);
#if 0
		if(!message)
			co_return false;
#endif

		// Just handle the message if there's no property data.
		if(header.payloadSize == 1 || header.payloadSize == 0) {
			co_await message->HandleClientMessage(server, client);
			co_return true;
		}

		// Read in the property buffer.
		// Fail if this fails
		if(!message->ReadProperties(buf))
			co_return false;

		co_await message->HandleClientMessage(server, client);
		co_return true;
	}

} // namespace ls