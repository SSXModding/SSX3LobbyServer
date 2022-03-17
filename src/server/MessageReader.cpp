//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "MessageReader.h"

#include "WireMessageHeader.h"
#include <byteswap.h>

#include <cstring>

namespace ls {


	// 2mb max payload size.
	// also taken from bustin in
	constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024) * 2;

	void MessageReader::ReadMessage(const std::vector<std::uint8_t>& buf, std::shared_ptr<Server> server, std::shared_ptr<Client> client) {
		WireMessageHeader header;
		std::vector<std::uint8_t> property_buf;

		if(buf.empty())
			return;

		// Read the message header
		memcpy(&header, &buf[0], sizeof(WireMessageHeader));

		// fix the endian on payload size
		header.payloadSize = LSNetworkToHost32(header.payloadSize);

		if(header.payloadSize > MAX_PAYLOAD_SIZE) {
			// TODO: Probably close the client connection, or throw an exception
			// 	to force the client to close.
			return;
		}

		auto message = ls::CreateMessageFromTypeCode(header.typeCode);
#if 0
		if(!message)
			return;
#endif

		// Just handle the message if there's no property data.
		if(header.payloadSize == 1 || header.payloadSize == 0)
			return message->HandleMessage(server, client);

		// Read in the property
		property_buf.resize(header.payloadSize);
		memcpy(&property_buf[0], &buf[sizeof(WireMessageHeader)], header.payloadSize);

		message->ReadProperties(property_buf);

		message->HandleMessage(server, client);
	}

} // namespace ls