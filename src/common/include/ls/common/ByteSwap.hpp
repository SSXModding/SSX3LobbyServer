//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_BYTESWAP_HPP
#define SSX3LOBBYSERVER_BYTESWAP_HPP

#include <bit>

namespace ls::common {

	// Compiler specific intrinsic detection; for now
	// GCC/Clang/MSVC are supported as we probably can only build on those.

#if defined(__GNUC__) || defined(__clang__)
	#define LS_SWAP_16(x) __builtin_bswap16(std::bit_cast<std::uint16_t>(x))
	#define LS_SWAP_32(x) __builtin_bswap32(std::bit_cast<std::uint32_t>(x))
	#define LS_SWAP_64(x) __builtin_bswap64(std::bit_cast<std::uint64_t>(x))
#elif defined(_MSC_VER)
	#include <intrin.h>
	#pragma intrinsic(_byteswap_ushort)
	#pragma intrinsic(_byteswap_ulong)
	#pragma intrinsic(_byteswap_uint64)

	#define LS_SWAP_16(x) _byteswap_ushort(std::bit_cast<std::uint16_t>(x))
	#define LS_SWAP_32(x) _byteswap_ulong(std::bit_cast<std::uint32_t>(x))
	#define LS_SWAP_64(x) _byteswap_uint64(std::bit_cast<std::uint64_t>(x))
#elif
	#error Unknown compiler; check in support for it
#endif

	template <class T>
		requires(sizeof(T) == 1)
	constexpr T ByteSwap(T swapped) {
		// No-op; a single byte doesn't need to be swapped.
		return swapped;
	}

	template <class T>
		requires(sizeof(T) == 2)
	constexpr T ByteSwap(T swapped) {
		return std::bit_cast<T>(LS_SWAP_16(swapped));
	}

	template <class T>
		requires(sizeof(T) == 4)
	constexpr T ByteSwap(T swapped) {
		return std::bit_cast<T>(LS_SWAP_32(swapped));
	}

	template <class T>
		requires(sizeof(T) == 8)
	constexpr T ByteSwap(T swapped) {
		return std::bit_cast<T>(LS_SWAP_64(swapped));
	}

	// generic overload/implementation that is slower but takes in an arbitrary buffer and swaps
	// 16 bits at a time?

	/**
	 * Swap a value from host endian to network endian, or vise versa.
	 */
	template <class T>
	constexpr T NetworkSwap(T value) {
		if constexpr(std::endian::native == std::endian::little)
			return ByteSwap(value);
		else
			return value;
	}

	template <class T>
	struct [[gnu::packed]] NetworkOrderValue {
		constexpr NetworkOrderValue() = default;
		constexpr NetworkOrderValue(const T& hostValue)
			: networkValue(NetworkSwap(hostValue)) {
		}

		operator T() const {
			return NetworkSwap(networkValue);
		}

	   private:
		T networkValue {};
	};

} // namespace ls::common

#undef LS_SWAP_16
#undef LS_SWAP_32
#undef LS_SWAP_64

#endif // SSX3LOBBYSERVER_BYTESWAP_HPP
