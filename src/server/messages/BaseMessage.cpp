//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "BaseMessage.h"

namespace ls {

	std::unordered_map<int, detail::MessageFactory>& MessageFactoryMap() {
		// crazy, I'll probably replace this with a sane thing or make this a macro
		// to implement COFU
		static std::remove_reference_t<decltype(MessageFactoryMap())> a;
		return a;
	}


	namespace detail {

		void RegisterMessage(int typeCode, MessageFactory factory) {
			auto& map = MessageFactoryMap();
			auto it = map.find(typeCode);

			if(it == map.end())
				return;

			map.insert({typeCode, factory});
		}

	}

	std::shared_ptr<MessageBase> CreateMessageFromTypeCode(int TypeCode) {
		auto& map = MessageFactoryMap();
		auto it = map.find(TypeCode);

		if(it == map.end())
			return nullptr;

		// else we have the factory so let's do it.
		return (it->second)();
	}

}