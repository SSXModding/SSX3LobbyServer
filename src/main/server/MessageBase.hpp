//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_MESSAGEBASE_HPP
#define SSX3LOBBYSERVER_MESSAGEBASE_HPP

#include <FourCC.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <asio/AsioConfig.hpp>

// Some notes:
//
// - The current factory/""reflection"" system heap-allocates EVERY parsed message.
//		Should an adapter Allocator like the PMR Allocators that allocates via a memory pool be used or something?
//		(don't really need performance, but we don't want spam parsed messages to DoS the service or require more resources than needed.)
//		(could also fix above with a ratelimit. which I may do tbh)
//
// - Refactor the factory to put messages in specific groups. The reader can then have a
//		MessageReader& AddGroup(ls::MessageGroup&) noexcept; (which can be chained)
//
//		then can do:
//			reader.AddGroup(ls::SystemMessages())
//				  .AddGroup(ls::BuddyMessages());
//
//		this would allow for buddy port and everything to not allow normal messages,
//		which is PROBABLY a good idea.
//
// - Get rid of MessageReader, Client can handle things itself (and saves 1 byte+padding)?
//

namespace ls {

	// fwd decls, don't bring these in just for existing or being used as shared_ptr args..
	struct Server;
	struct Client;

	/**
	 * Base class for lobby messages.
	 */
	struct MessageBase {
		virtual ~MessageBase() = default;

		/**
		 * Serialize this message to a output buffer.
		 */
		void Serialize(std::vector<std::uint8_t>& outBuf) const;

		/**
		 * Call this when creating a response message to immediately
		 * create any properties which need to be sent with this message.
		 */
		virtual void CreateDefaultProperties();

		/**
		 * Get a specific message property for writing.
		 */
		std::string& GetProperty(const std::string& name);

	   protected:
		friend struct MessageReader;

		/**
		 * Called when the message parsing is finished, to handle the message.
		 * Base version does nothing.
		 * Override to do message specific handling.
		 */
		virtual Awaitable<void> HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client);

		/**
		 * Read the serialized properties from a buffer.
		 *
		 * \param[in] inBuf buffer to read serialized properties from (to complete this message)
		 * \param[in] client Client which this message is from.
		 * \return True on success, false on failure.
		 */
		bool ReadProperties(const std::vector<std::uint8_t>& inBuf);

		/**
		 * The type code.
		 * This is expected to be set by the read operation.
		 */
		uint32_t typeCode;

		/**
		 * All properties.
		 */
		std::unordered_map<std::string, std::string> properties;
	};

	namespace detail {
		using MessageFactory = std::shared_ptr<MessageBase> (*)();
		void RegisterMessage(uint32_t typeCode, MessageFactory factory);

		// Registers a message into the internal map.
		template <class T, uint32_t TypeCode>
		struct MessageRegistrar {
			MessageRegistrar() {
				RegisterMessage(TypeCode, &MessageRegistrar<T, TypeCode>::createMessage);
			}

		   private:
			// the factory function...
			static std::shared_ptr<MessageBase> createMessage() {
				return std::make_shared<T>();
			}
		};

	} // namespace detail

	/**
	 * Register a message into the "reflection" system.
	 */
#define LSRegisterMessage(TypeCode, T)                                \
	static ls::detail::MessageRegistrar<T, TypeCode> __registrar_##T; \
	auto* __hack_forcedep__##T = &__registrar_##T;

	/**
	 * Create a message from type code.
	 * Essentially a very very limited form of "reflection".
	 *
	 * Returns a "debug" message or nullptr if the factory collection
	 * doesn't have the specific type code in it.
	 */
	std::shared_ptr<MessageBase> CreateMessageFromTypeCode(uint32_t TypeCode);

} // namespace ls

#endif // SSX3LOBBYSERVER_MESSAGEBASE_HPP
