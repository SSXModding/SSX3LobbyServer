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

#include "WireMessageHeader.h"

// TODO: probably move this into a common header
#ifdef __GNUC__
	#define HostToNetwork32(x) __bswap_32(x)
#else
	#error lazy please wait for me to implement youre are compiler support
#endif

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

	// TODO for debugging: Make a "default" message which doesn't do any real processing,
	// and just prints out the message and its properties?

	struct DebugMessage : MessageBase {
		void HandleMessage(std::shared_ptr<Client> client) override {
			// TODO: debug stuff
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
		outBuf.resize(sizeof(WireMessageHeader) + header.payloadSize);

		// Write to the output buffer now.
		memcpy(&outBuf[0], &header, sizeof(WireMessageHeader));
		memcpy(&outBuf[sizeof(WireMessageHeader)], total.data(), total.length());
	}

} // namespace ls