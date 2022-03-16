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

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

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
		 * Read a message from a buffer.
		 */
		void Read(const std::vector<std::uint8_t>& inBuf, std::shared_ptr<Client> client);

		/**
		 * Called when the message parsing is finished. Base version does nothing.
		 * Override to do message specific handling.
		 */
		virtual void HandleMessage(std::shared_ptr<Client> client);

		/**
		 * Call this when creating a response message to immediately
		 * create any properties which need to be sent with this message.
		 */
		virtual void CreateDefaultProperties();

		/**
		 * Get a specific message property.
		 */
		const std::string& GetProperty(std::string_view name) const;

		/**
		 * Get a specific message property for writing.
		 */
		std::string& GetProperty(std::string_view name);

		/**
		 * Create a property.
		 */
		std::string& CreateProperty(std::string_view name);

	   protected:
		/**
		 * The type code.
		 * This is expected to be set by the read operation.
		 */
		int typeCode;

		/**
		 * All properties.
		 */
		std::unordered_map<std::string, std::string> properties;
	};

	namespace detail {
		using MessageFactory = std::shared_ptr<MessageBase>(*)();
		void RegisterMessage(int typeCode, MessageFactory factory);

		// Registers a message into the internal map.
		template<class T>
		struct MessageRegistrar {
			MessageRegistrar(int TypeCode) {
				detail::RegisterMessage(TypeCode, &MessageRegistrar::createMessage);
			}

		   private:
			// the factory function...
			static std::shared_ptr<MessageBase> createMessage() {
				return std::make_shared<T>();
			}
		};

#define REGISTER_MESSAGE(TypeCode, T) static ls::detail::MessageRegistrar<T> __registrar__##T(TypeCode);

	}

	// Creates a message from the internal message factory, or a null shared_ptr on error.
	std::shared_ptr<MessageBase> CreateMessageFromTypeCode(int TypeCode);

}

#endif // SSX3LOBBYSERVER_BASEMESSAGE_H
