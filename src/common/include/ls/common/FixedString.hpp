//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <cstdint>

#ifndef SSX3LOBBYSERVER_FIXEDSTRING_HPP
#define SSX3LOBBYSERVER_FIXEDSTRING_HPP

namespace ls::common {

	/**
	 * A compile-time string. Usable as a C++20 cNTTP.
	 */
	template <std::size_t N>
	struct FixedString {
		char buf[N + 1] {};

		constexpr FixedString(const char* s) { // NOLINT
			for(unsigned i = 0; i != N; ++i)
				buf[i] = s[i];
		}

		constexpr operator const char*() const { // NOLINT
			return buf;
		}

		[[nodiscard]] constexpr std::size_t Length() const {
			return N;
		}
	};

	template <std::size_t N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;

} // namespace ls::common

#endif // SSX3LOBBYSERVER_FIXEDSTRING_HPP
