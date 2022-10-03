//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "MessageBase.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <ByteSwap.hpp>
#include <CofuSingleton.hpp>

#include "asio/AsioConfig.hpp"
#include "WireMessageHeader.hpp"

namespace ls {

	// TODO: move message factory to separate implementation file
	namespace detail {

		static ls::CofuSingleton<std::unordered_map<uint32_t, ls::detail::MessageFactory>> MessageFactoryMap;

		void RegisterMessage(uint32_t typeCode, MessageFactory factory) {
			auto& map = MessageFactoryMap();
			auto it = map.find(typeCode);

			if(it != map.end())
				return;

			map.insert({ typeCode, factory });
		}

	} // namespace detail

	std::shared_ptr<MessageBase> CreateMessageFromTypeCode(uint32_t TypeCode) {
		const auto& map = detail::MessageFactoryMap();
		auto it = map.find(TypeCode);

		/**
		 * Internal message type used when there's no assigned
		 * handler for a message. Only used for debugging.
		 * May be phased out when everything's all working.
		 */
		struct DebugMessage : MessageBase {
			explicit DebugMessage(uint32_t TypeCode) {
				this->typeCode = TypeCode;
			}

			Awaitable<void> HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) override {
				auto* fccbytes = ((uint8_t*)&typeCode);

				spdlog::info("Debug Message FourCC lo: \"{:c}{:c}{:c}{:c}\"", fccbytes[0], fccbytes[1], fccbytes[2], fccbytes[3]);
				spdlog::info("Debug Message Properties:");

				for(auto [key, value] : properties)
					spdlog::info("{}: {}", key, value);
			}
		};

		if(it == map.end())
#if 1
			return std::make_shared<DebugMessage>(TypeCode);
#else
			return nullptr;
#endif

		// else we have the factory so let's do it.
		return (it->second)();
	}

	void MessageBase::Serialize(std::vector<std::uint8_t>& outBuf) const {
		WireMessageHeader header;
		std::string total;

		// Serialize all properties.
		for(auto [key, value] : properties)
			total += fmt::format("{}={}\n", key, value);

		// Null terminate the property data.
		total.push_back('\0');

		auto len = total.length() - 1;

		// Fill in the header.
		header.typeCode = this->typeCode; // TODO: do I need to HostToNetwork32 this?
		header.typeCodeHi = 0x00000000;	  // fill
		header.payloadSize = LSHostToNetwork32(len);

		// Resize the output buffer so we can just copy our hard work to it.
		outBuf.resize(sizeof(WireMessageHeader) + total.length());

		// Write to the output buffer now.
		memcpy(&outBuf[0], &header, sizeof(WireMessageHeader));
		memcpy(&outBuf[sizeof(WireMessageHeader)], total.data(), total.length());
	}

	bool MessageBase::ReadProperties(const std::vector<std::uint8_t>& inBuf) {
		// Nothing to parse, so return success
		if(inBuf.empty())
			return true;

		// Current state of the reader state machine (see below.)

		std::string key;
		std::string val;

		size_t inputIndex = 0;

		enum class ReaderState {
			InKey,	///< The state machine is currently parsing a key.
			InValue ///< The state machine is currently parsing a value.
		} state { ReaderState::InKey };

		// Parse all properties, using a relatively simple state machine.
		//
		// State transition mappings:
		// = - from key to value state (if in key state)
		// \n - from value to key state (if in value state, otherwise error)

		while(inputIndex != inBuf.size()) {
			switch(inBuf[inputIndex]) {
				case '=':
					if(state == ReaderState::InKey) {
						state = ReaderState::InValue;
						break;
					} else {
						// If we're in the value state, we're allowed to nest = signs, I think.
						// if not, then this is PROBABLY an error state.
						val += static_cast<char>(inBuf[inputIndex]);
					}
					break;

				case '\n':
					if(state == ReaderState::InValue) {
						//printf("state transition - to key\n");

						properties[key] = val;

						// Reset the state machine, to read another property.
						key.clear();
						val.clear();
						state = ReaderState::InKey;
						break;
					} else {
						// If we get here in the key state, this is DEFINITELY an error.
						return false;
					}

					// Any other characters are not important to the state machine,
					// and are instead written to the given staging string for the current
					// state machine state.
				default:
					switch(state) {
						case ReaderState::InKey:
							key += static_cast<char>(inBuf[inputIndex]);
							break;
						case ReaderState::InValue:
							// Skip past quotation marks.
							// I dunno if it's really needed.
							// (For reference: SSX3 Dirtysock does the same thing, even including ').
							if(static_cast<char>(inBuf[inputIndex]) == '\"')
								break;

							val += static_cast<char>(inBuf[inputIndex]);
							break;
					}
					break;
			}

			inputIndex++;
		}

		// Parse succeeded
		return true;
	}

	Awaitable<void> MessageBase::HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) {
		co_return;
	}

	void MessageBase::CreateDefaultProperties() {
	}

	std::string& MessageBase::GetProperty(const std::string& name) {
		return properties[name];
	}

} // namespace ls