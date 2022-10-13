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

#include <ls/asio/AsioConfig.hpp>
#include <ls/common/FourCC.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Some notes:
//
// - Refactor the factory to put messages in specific groups.
//	 The client can then check what groups it's allowed to use,
//	 and if a given message fourcc isn't in that list,
//	 try to read the next message. This can be done without even allocating
//	 the attempted message, so heap concerns don't really matter as much.
//

namespace ls {

	// fwd decls, don't bring these in just for existing or being used as shared_ptr args..
	struct Server;
	struct Client;

	/**
	 * Base class for Dirtysock messages.
	 */
	struct MessageBase {
		virtual ~MessageBase() = default;

		/**
		 * Serialize this message to a output buffer.
		 *
		 * \param[out] outBuf Output buffer to serialize to.
		 */
		void Serialize(std::vector<std::uint8_t>& outBuf) const;

		/**
		 * Get a specific message property for writing.
		 */
		[[nodiscard]] std::string& GetProperty(const std::string& name) {
			return properties[name]; // Okay for the mutable overload to use "suprising" behaviour of *_map
		}

		/**
		 * Get a property for reading.
		 */
		[[nodiscard]] const std::string& GetProperty(const std::string& name) const {
			return properties.at(name);
		}

		/**
		 * Get the type code of this message.
		 *
		 * \return The message type code.
		 */
		virtual std::uint32_t TypeCode() const = 0;

		/**
		 * Create a message.
		 */
		static std::shared_ptr<MessageBase> Create(std::uint32_t typeCode);

		/**
		 * Called when the message parsing is finished, to handle the message.
		 * Base version does nothing.
		 * Override to do message specific handling.
		 *
		 * It is permitted to call ASIO operations in this handler.
		 */
		virtual Awaitable<void> HandleClientMessage(std::shared_ptr<Client> client) = 0;

		/**
		 * Read the serialized properties from a buffer.
		 *
		 * \param[in] inBuf buffer to read serialized properties from (to complete this message)
		 * \param[in] client Client which this message is from.
		 * \return True on success, false on failure.
		 */
		bool ReadProperties(const std::vector<std::uint8_t>& inBuf);

	   private:
		template <common::FixedString fourcc, class T>
		friend struct Message;

		static auto& FactoryMap() {
			static std::unordered_map<uint32_t, std::shared_ptr<MessageBase> (*)()> map;
			return map;
		}

		/**
		 * All properties.
		 */
		std::unordered_map<std::string, std::string> properties;
	};

	/**
	 * CRTP helper class for messages.
	 * Implements required virtuals; automatically registers, the whole shebang.
	 *
	 * Inherit from this.
	 */
	template <common::FixedString fccString, class T>
	struct Message : public ls::MessageBase {
		static constexpr auto TYPE_CODE = common::FourCC<fccString>();

		constexpr explicit Message()
			: MessageBase() {
			static_cast<void>(registered);
		}

		std::uint32_t TypeCode() const override {
			return TYPE_CODE;
		}

	   private:
		static bool Register() {
			MessageBase::FactoryMap()[Message::TYPE_CODE] = []() -> std::shared_ptr<MessageBase> {
				return std::make_shared<T>();
			};

			return true;
		}

		static inline bool registered = Register();
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_MESSAGEBASE_HPP
