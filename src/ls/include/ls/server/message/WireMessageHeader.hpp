//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_WIREMESSAGEHEADER_HPP
#define SSX3LOBBYSERVER_WIREMESSAGEHEADER_HPP

#include <cstdint>
#include <ls/common/ByteSwap.hpp>

namespace ls {

	/**
	 * The on-wire message header.
	 */
	struct [[gnu::packed]] WireMessageHeader {
		/**
		 * Message type code.
		 */
		std::uint32_t typeCode {};

		/**
		 * High fourcc. Seems to only be used by responses.
		 */
		std::uint32_t typeCodeHi {};

		/**
		 * The size of the payload, which immediately follows this structure.
		 */
		common::NetworkOrderValue<std::uint32_t> payloadSize {};
	};

	// Sanity checking.
	static_assert(sizeof(WireMessageHeader) == 12, "Wire message header size is invalid");

} // namespace ls

#endif // SSX3LOBBYSERVER_WIREMESSAGEHEADER_HPP
