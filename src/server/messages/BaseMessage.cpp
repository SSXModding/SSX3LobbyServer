//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "BaseMessage.h"

#include <fmt/core.h>

#include "../common/fourcc.h"
#include "WireMessageHeader.h"

// TODO: probably move this into a common header
#ifdef __GNUC__

	#if __BYTE_ORDER__ == __LITTLE_ENDIAN
		#define HostToNetwork32(x) __bswap_32(x)
		#define NetworkToHost32(x) __bswap_32(x)
	#else
		#define HostToNetwork32(x) (x)
		#define NetworkToHost32(x) (x)
	#endif

#else
	#error lazy please wait for me to implement youre are compiler support
#endif

namespace ls {

	// 2mb max payload size.
	// also taken from bustin in
	constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024) * 2;

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
		DebugMessage() {
			this->typeCode = FourCCValue("test");
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
			return std::make_shared<DebugMessage>();
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
		header.payloadSize = HostToNetwork32(total.length());

		// Resize the output buffer so we can just copy our hard work to it.
		outBuf.resize(sizeof(WireMessageHeader) + total.length());

		// Write to the output buffer now.
		memcpy(&outBuf[0], &header, sizeof(WireMessageHeader));
		memcpy(&outBuf[sizeof(WireMessageHeader)], total.data(), total.length());
	}

	void MessageBase::Read(const std::vector<std::uint8_t>& inBuf, std::shared_ptr<Client> client) {
		// TODO this probably will have to be moved elsewhere...
		WireMessageHeader header;
		std::string rawPropertyBlob;

		if(inBuf.empty())
			return;

		// Read the message header
		memcpy(&header, &inBuf[0], sizeof(WireMessageHeader));

		// fix the endian on payload size
		header.payloadSize = NetworkToHost32(header.payloadSize);

		if(header.payloadSize > MAX_PAYLOAD_SIZE) {
			// TODO: Probably close the client connection.
			return;
		}

		// Read in the raw property blob from the packet before processing it.

		rawPropertyBlob.resize(header.payloadSize);
		memcpy(&rawPropertyBlob[0], &inBuf[sizeof(WireMessageHeader)], header.payloadSize);

		std::string key;
		std::string val;

		uint32_t Index = 0;
		bool InKey = true;

		// Parse all properties.
		while(Index != header.payloadSize) {
			if(rawPropertyBlob[Index] == '=' && InKey) {
				// printf("key is %s\n", key.c_str());
				InKey = false;
				Index++;
				continue;
			}

			if(rawPropertyBlob[Index] == '\n' && !InKey) {
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
				key += rawPropertyBlob[Index];
			else {
				// Skip past quotation marks. Bustin in does this,
				// I dunno if it's really needed.
				if(rawPropertyBlob[Index] == '\"') {
					Index++;
					continue;
				}

				val += rawPropertyBlob[Index];
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