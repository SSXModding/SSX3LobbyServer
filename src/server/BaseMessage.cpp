//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "BaseMessage.h"
#include "WireMessageHeader.h"

#include <fmt/core.h>
#include <byteswap.h>

namespace ls {

	namespace {

		std::unordered_map<uint32_t, detail::MessageFactory>& MessageFactoryMap() {
			// crazy, I'll probably replace this with a sane thing or make this a macro
			// to implement COFU
			static std::remove_reference_t<decltype(MessageFactoryMap())> a;
			return a;
		}

		std::string FormatKeyVal(std::string_view key, std::string_view value) {
			return fmt::format("{}={}\n", key, value);
		}
	} // namespace

	namespace detail {

		void RegisterMessage(uint32_t typeCode, MessageFactory factory) {
			auto& map = MessageFactoryMap();
			auto it = map.find(typeCode);

			if(it == map.end())
				return;

			map.insert({ typeCode, factory });
		}

	} // namespace detail

	struct DebugMessage : MessageBase {
		explicit DebugMessage(uint32_t TypeCode) {
			this->typeCode = TypeCode;
		}

		void HandleMessage(std::shared_ptr<Client> client) override {
			auto* fccbytes = ((uint8_t*)&typeCode);

			printf("typecode: %c%c%c%c\n", fccbytes[0], fccbytes[1], fccbytes[2], fccbytes[3]);
			printf("properties:\n");
			for(auto [key, value] : properties) {
				printf("\t%s : %s\n", key.c_str(), value.c_str());
			}
		}
	};

	std::shared_ptr<MessageBase> CreateMessageFromTypeCode(uint32_t TypeCode) {
		auto& map = MessageFactoryMap();
		auto it = map.find(TypeCode);

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
		for(auto [key, value] : properties) {
			total += FormatKeyVal(key, value);
		}

		// Null terminate the property data.
		total.push_back('\0');

		// Fill in the header.
		header.typeCode = this->typeCode; // TODO: do I need to HostToNetwork32 this?
		header.unknown = 0x00000000;	  // fill
		header.payloadSize = LSHostToNetwork32(total.length());

		// Resize the output buffer so we can just copy our hard work to it.
		outBuf.resize(sizeof(WireMessageHeader) + total.length());

		// Write to the output buffer now.
		memcpy(&outBuf[0], &header, sizeof(WireMessageHeader));
		memcpy(&outBuf[sizeof(WireMessageHeader)], total.data(), total.length());
	}

	void MessageBase::ReadProperties(const std::vector<std::uint8_t>& inBuf, std::shared_ptr<Client> client) {
		if(inBuf.empty())
			return;

		std::string key;
		std::string val;

		uint32_t Index = 0;
		bool InKey = true;

		// Parse all properties.
		while(Index != inBuf.size()) {
			if(inBuf[Index] == '=' && InKey) {
				// printf("key is %s\n", key.c_str());
				InKey = false;
				Index++;
				continue;
			}

			if(inBuf[Index] == '\n' && !InKey) {
				// printf("finished read of %s val %s\n", key.c_str(), val.c_str());
				InKey = true;
				properties[key] = val;

				key.clear();
				val.clear();
				Index++;
				continue;
			}

			// Write to the appropriate value for the
			// current state.
			if(InKey)
				key += inBuf[Index];
			else {
				// Skip past quotation marks. Bustin in does this,
				// I dunno if it's really needed.
				if(inBuf[Index] == '\"') {
					Index++;
					continue;
				}

				val += inBuf[Index];
			}

			Index++;
		}

		// We've finally gotten it.
		HandleMessage(client);
	}

	void MessageBase::HandleMessage(std::shared_ptr<Client> client) {
	}

	void MessageBase::CreateDefaultProperties() {
	}

	std::string& MessageBase::GetProperty(const std::string& name) {
		return properties[name];
	}

} // namespace ls