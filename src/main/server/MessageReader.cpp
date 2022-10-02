//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "MessageReader.hpp"

#include <ByteSwap.hpp>
#include <cstring>

namespace ls {


	// 2mb max payload size.
	// also taken from bustin in
	constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024) * 2;

	std::optional<WireMessageHeader> MessageReader::ReadHeader(const std::uint8_t* buf) noexcept {
		WireMessageHeader ret;

		if(buf == nullptr)
			return std::nullopt;

		// Read the message header
		memcpy(&ret, &buf[0], sizeof(WireMessageHeader));

		// fix the endian on payload size
		ret.payloadSize = LSNetworkToHost32(ret.payloadSize);

		if(ret.payloadSize > MAX_PAYLOAD_SIZE) {
			// TODO: Probably close the client connection, or throw an exception
			// 	to force the client to close.
			return std::nullopt;
		}

		return ret;
	}

	bool MessageReader::ReadAndHandleMessage(const WireMessageHeader& header, const std::vector<std::uint8_t>& buf, std::shared_ptr<Server> server, std::shared_ptr<Client> client) noexcept {
		// Create the message instance from the message factory
		auto message = ls::CreateMessageFromTypeCode(header.typeCode);
#if 0
		if(!message)
			return;
#endif

		// Just handle the message if there's no property data.
		if(header.payloadSize == 1 || header.payloadSize == 0) {
			message->HandleClientMessage(server, client);
			return true;
		}

		// Read in the property buffer.
		// Fail if this fails
		if(!message->ReadProperties(buf))
			return false;

		message->HandleClientMessage(server, client);
		return true;
	}

} // namespace ls