//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_MESSAGEREADER_HPP
#define SSX3LOBBYSERVER_MESSAGEREADER_HPP

#include <MessageBase.hpp>
#include <optional>
#include <WireMessageHeader.hpp>

namespace ls {

	/**
	 * Reader for Dirtysock messages.
	 */
	struct MessageReader {

		/**
		 * Read the header of a dirtysock message.
		 * \param[in] buf Header buffer.
		 * \return A header on success, nullopt otherwise.
		 */
		std::optional<WireMessageHeader> ReadHeader(const std::uint8_t* buf) noexcept;

		/**
		 * Read the rest of and then handle (dispatching to the HandleClientMessage() function) a received message.
		 *
		 * \param[in] header message header.
		 * \param[in] buf Property buffer.
		 * \returns True if message parsed and handled successfully; false otherwise.
		 */
		bool ReadAndHandleMessage(const WireMessageHeader& header, const std::vector<std::uint8_t>& buf, std::shared_ptr<Server> server, std::shared_ptr<Client> client) noexcept;

	};

}

#endif // SSX3LOBBYSERVER_MESSAGEREADER_HPP
