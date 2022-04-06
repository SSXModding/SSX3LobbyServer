//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "MessageBase.h"

#include <byteswap.h>
#include <fmt/core.h>
#include <singleton.h>
#include <spdlog/spdlog.h>

#include "WireMessageHeader.h"

namespace ls {

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
		auto* fccbytes = ((uint8_t*)&TypeCode);

		auto& map = detail::MessageFactoryMap();
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

			void HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) override {
				auto* fccbytes = ((uint8_t*)&typeCode);

				spdlog::info("fourcc lo: \"{:c}{:c}{:c}{:c}\"", fccbytes[0], fccbytes[1], fccbytes[2], fccbytes[3]);
				spdlog::info("properties:");

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

	void MessageBase::ReadProperties(const std::vector<std::uint8_t>& inBuf) {
		if(inBuf.empty())
			return;

		std::string key;
		std::string val;

		uint32_t inputIndex = 0;

		// state of the reader state machine (see below)
		enum class ReaderState {
			InKey,
			InValue
		} state{ReaderState::InKey};

		// Parse all properties, using a relatively simple state machine.
		//
		// State transition mappings:
		// = - from key to value state (if in key state)
		// \n - from value to key state (if in value state)
		//

		while(inputIndex != inBuf.size()) {
			switch(inBuf[inputIndex]) {

				case '=':
					if(state == ReaderState::InKey) {
						state = ReaderState::InValue;
						break;
					} else {
						// If we're in the value state, we're allowed to nest = signs, I think.
						val += inBuf[inputIndex];
					}
					break;

				case '\n':
					if(state == ReaderState::InValue) {
						printf("state transition - to key\n");
						properties[key] = val;
						state = ReaderState::InKey;
						key.clear();
						val.clear();
						break;
					}
					break;

				default:
					switch(state) {
						case ReaderState::InKey:
							key += inBuf[inputIndex];
							break;
						case ReaderState::InValue:
							// Skip past quotation marks.
							// I dunno if it's really needed.
							// (For reference: SSX3 Dirtysock does the same thing, even including ').
							if(inBuf[inputIndex] == '\"')
								break;

							val += inBuf[inputIndex];
							break;
					}
					break;
			}

			inputIndex++;
		}
	}

	void MessageBase::HandleClientMessage(std::shared_ptr<Server> server, std::shared_ptr<Client> client) {
	}

	void MessageBase::CreateDefaultProperties() {
	}

	std::string& MessageBase::GetProperty(const std::string& name) {
		return properties[name];
	}

} // namespace ls