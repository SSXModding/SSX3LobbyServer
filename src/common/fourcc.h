//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_FOURCC_H
#define SSX3LOBBYSERVER_FOURCC_H

#include <cstdint>

namespace ls {

	/**
	 * Make a FourCC value at compile time, with a given input string.
	 */
	constexpr std::uint32_t FourCCValue(const char fourCc[5]) {
	//	return (fourCc[0] << 24) | (fourCc[1] << 16) | (fourCc[2] << 8) | fourCc[3];
		return (fourCc[3] << 24) | (fourCc[2] << 16) | (fourCc[1] << 8) | fourCc[0];
	}

	// TODO: 64-bit version which returns a u64 (if required?)

}

#endif // SSX3LOBBYSERVER_FOURCC_H
