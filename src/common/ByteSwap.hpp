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

// TODO: implement msvc & generic C intrins
#ifdef __GNUC__

	#if __BYTE_ORDER__ == __LITTLE_ENDIAN
		#define LSHostToNetwork32(x) __bswap_32(x)
		#define LSNetworkToHost32(x) __bswap_32(x)
	#else
		#define LSHostToNetwork32(x) (x)
		#define LSNetworkToHost32(x) (x)
	#endif

#else


	// generic C intrins
	#error lazy please wait for me to implement youre are compiler support
#endif

#endif // SSX3LOBBYSERVER_BYTESWAP_HPP
