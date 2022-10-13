//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_FOURCC_HPP
#define SSX3LOBBYSERVER_FOURCC_HPP

#include <bit>
#include <cstdint>

#include "FixedString.hpp"

namespace ls::common {

	/**
	 * A multi-endian, compile-time FourCC generator.
	 * You love to see it.
	 */
	template <FixedString fccString, std::endian Endian = std::endian::little>
	consteval std::uint32_t FourCC() {
		static_assert(fccString.Length() == 4, "Provided string is not a FourCC");

		switch(Endian) {
			case std::endian::little:
				return (fccString[0]) | (fccString[1] << 8) | (fccString[2] << 16) | (fccString[3] << 24);

			case std::endian::big:
				return (fccString[0] << 24) | (fccString[1] << 16) | (fccString[2] << 8) | fccString[3];

			// I wish there was a better way to do compile time errors :(
			default:
				throw "Invalid endian provided? How'd you do that?"; // NOLINT
		}
	}

	// TODO: 64-bit version which returns a u64 (if required?)
} // namespace ls::common

#endif // SSX3LOBBYSERVER_FOURCC_HPP
