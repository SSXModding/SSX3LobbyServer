//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_BASEMESSAGE_H
#define SSX3LOBBYSERVER_BASEMESSAGE_H

#include <fourcc.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Some notes:
// - The current factory/""reflection"" system heap allocs EVERY parsed message.
//		Should an adapter Allocator like the PMR Allocators that allocates via a memory pool be used or something?
//		(don't really need performance, but we don't want spam parsed messages to DoS the service or require more resources than needed.)
//
// 		We could maybe also do locked singleton access to a message, which would reduce allocations considerably but maybe
//		make program logic a bit more difficult (as well as serializing access, making the general outbound performance possibly about the same
//		as if the server were single-threaded, if not worse. BUT, less allocs!)
//
// - The current system needs a hack on Linux to work, which I don't really like.
// 		An alternative I've thought is to provide a ls::InitMessages() API which calls all the internal init routines.
//		This would work but is more manual (an error-prone point!) and a little grodier. Still, I'm considering it.
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
		 * Called when the message parsing is finished. Base version does nothing.
		 * Override to do message specific handling.
		 */
		virtual void HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client);

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
		 * Read the serialized properties from a buffer.
		 *
		 * \param[in] inBuf buffer to read serialized properties from (to complete this message)
		 * \param[in] client Client which this message is from.
		 */
		void ReadProperties(const std::vector<std::uint8_t>& inBuf);

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

#define LSRegisterMessage(TypeCode, T) static ls::detail::MessageRegistrar<T, TypeCode> __registrar_##T; \
	auto* __hack_forcedep__##T = &__registrar_##T;

	/**
	 * Create a message from type code.
	 * Essentially a very very limited form of "reflection".
	 *
	 * Returns a "debug" message or nullptr if the factory collection
	 * doesn't have the specific type code in it.
	 */
	std::shared_ptr<MessageBase>
	CreateMessageFromTypeCode(uint32_t TypeCode);

} // namespace ls

#endif // SSX3LOBBYSERVER_BASEMESSAGE_H
