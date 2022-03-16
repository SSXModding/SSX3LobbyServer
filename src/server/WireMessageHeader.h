//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_WIREMESSAGEHEADER_H
#define SSX3LOBBYSERVER_WIREMESSAGEHEADER_H

#include <cstdint>

namespace ls {

	/**
	 * The on-wire message header.
	 */
	struct WireMessageHeader {
		/**
		 * Message type code.
		 */
		uint32_t typeCode{};

		/**
		 * Unknown. Could be padding, unless it isn't? Bustin In sets it to all zeros.
		 * It seems to also allow 64bit typecodes too though.
		 */
		uint32_t unknown{};

		/**
		 * The size of the payload, which immediately follows this structure.
		 */
		uint32_t payloadSize{};
	};

	// Sanity checking.
	static_assert(sizeof(WireMessageHeader) == 12, "Wire message header size is invalid");

}

#endif // SSX3LOBBYSERVER_WIREMESSAGEHEADER_H
