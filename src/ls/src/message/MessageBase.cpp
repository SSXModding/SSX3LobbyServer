//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <ls/asio/AsioConfig.hpp>
#include <ls/server/message/MessageBase.hpp>
#include <ls/server/message/WireMessageHeader.hpp>

namespace ls {

	std::shared_ptr<MessageBase> MessageBase::Create(uint32_t TypeCode) {
		const auto& map = MessageBase::FactoryMap();
		auto it = map.find(TypeCode);

		/**
		 * Internal message type used when there's no assigned
		 * handler for a message. Only used for debugging.
		 * May be phased out when everything's all working.
		 */
		struct DebugMessage : MessageBase {
			explicit DebugMessage(uint32_t TypeCode) {
				myTypeCode = TypeCode;
			}

			std::uint32_t TypeCode() const override {
				return myTypeCode;
			}

			Awaitable<void> HandleClientMessage(std::shared_ptr<Client> client) override {
				auto* fccbytes = ((uint8_t*)&myTypeCode);

				spdlog::info("Debug Message FourCC lo: \"{:c}{:c}{:c}{:c}\"", fccbytes[0], fccbytes[1], fccbytes[2], fccbytes[3]);
				spdlog::info("Debug Message Properties:");

				for(auto [key, value] : properties)
					spdlog::info("{}: {}", key, value);

				co_return;
			}

			uint32_t myTypeCode;
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
		std::string serializedProperties;

		// Reserve a sane amount, to avoid allocations when serializing properties
		// (in most cases; larger messages MIGHT still cause some allocation pressure.)
		serializedProperties.reserve(512);

		// Serialize properties
		{
			auto i = properties.size();
			for(auto [key, value] : properties)
				if(--i != 0)
					serializedProperties += fmt::format("{}={}\n", key, value);
				else
					serializedProperties += fmt::format("{}={}", key, value);
		}

		// Null terminate the property data.
		serializedProperties.push_back('\0');

		// Create an appropriate header for the data.
		WireMessageHeader header {
			.typeCode = TypeCode(),
			.typeCodeHi = 0,
			.payloadSize = serializedProperties.length() - 1
		};

		auto fullLength = sizeof(WireMessageHeader) + serializedProperties.length();

		// Resize the output buffer to the right size
		outBuf.resize(fullLength);

		// Write to the output buffer now.
		memcpy(&outBuf[0], &header, sizeof(WireMessageHeader));
		memcpy(&outBuf[sizeof(WireMessageHeader)], serializedProperties.data(), serializedProperties.length());
	}

	bool MessageBase::ReadProperties(const std::vector<std::uint8_t>& inBuf) {
		// Nothing to parse,
		// which isn't exclusively a failure condition.
		if(inBuf.empty())
			return true;

		std::string key;
		std::string val;

		size_t inputIndex = 0;

		// TODO: Investigate rewriting this using ragel?

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
						// printf("state transition - to key\n");

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

} // namespace ls