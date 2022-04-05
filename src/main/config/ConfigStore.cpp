//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "ConfigStore.h"


namespace ls {

	std::optional<ConfigStore::ConfigValue> ConfigStore::GetValue(const ConfigKey& key) {
		auto it = values.find(key);

		// value not found
		if(it == values.end())
			return std::nullopt;

		return it->second;
	}

	void ConfigStore::SetValue(const ConfigKey& key, const ConfigValue& value) {
		values[key] = value;
	}


}