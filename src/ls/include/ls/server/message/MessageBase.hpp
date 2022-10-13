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

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <ls/asio/AsioConfig.hpp>
#include <ls/common/FourCC.hpp>

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

namespace ls {

	// fwd decls, don't bring these in just for existing or being used as shared_ptr args..
	struct Server;
	struct Client;

	/**
	 * Base class for lobby messages.
	 */
	struct MessageBase  {
		virtual ~MessageBase() = default;

		/**
		 * Serialize this message to a output buffer.
		 */
		void Serialize(std::vector<std::uint8_t>& outBuf) const;

		/**
		 * Get a specific message property for writing.
		 */
		[[nodiscard]] std::string& GetProperty(const std::string& name) {
			return properties.at(name);
		}

		[[nodiscard]] const std::string& GetProperty(const std::string& name) const {
			return properties.at(name);
		}

		virtual std::uint32_t TypeCode() const = 0;

		/**
		 * Create a message.
		 */
		static std::shared_ptr<MessageBase> Create(std::uint32_t typeCode);

	   protected:
		friend struct MessageReader;

		/**
		 * Called when the message parsing is finished, to handle the message.
		 * Base version does nothing.
		 * Override to do message specific handling.
		 */
		virtual Awaitable<void> HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) = 0;

		/**
		 * Read the serialized properties from a buffer.
		 *
		 * \param[in] inBuf buffer to read serialized properties from (to complete this message)
		 * \param[in] client Client which this message is from.
		 * \return True on success, false on failure.
		 */
		bool ReadProperties(const std::vector<std::uint8_t>& inBuf);

		/**
		 * All properties.
		 */
		std::unordered_map<std::string, std::string> properties;

	   private:
		template<common::FixedString fourcc, class T>
		friend struct Message;

		static auto& registeredMap() {
			static std::unordered_map<uint32_t, std::shared_ptr<MessageBase>(*)()> map;
			return map;
		}
	};

	/**
	 * CRTP helper class for messages. Implements required virtuals; automatically registers,
	 * the whole shebang.
	 */
	template<common::FixedString fourcc, class T>
	struct Message : public ls::MessageBase {
		static constexpr auto TYPE_CODE = common::FourCC<fourcc>();

		constexpr explicit Message()
			: MessageBase() {
			(void)registered;
		}

		std::uint32_t TypeCode() const override {
			return TYPE_CODE;
		}

	   private:
		static bool Register() {
			MessageBase::registeredMap()[Message::TYPE_CODE] = []() -> std::shared_ptr<MessageBase> {
				return std::make_shared<T>();
			};

			return true;
		}

		static inline bool registered = Register();
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_MESSAGEBASE_HPP
