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

	bool ConfigStore::HasValue(const ConfigKey& key) const {
		return values.find(key) != values.end();
	}

	std::optional<ConfigStore::ConfigValue> ConfigStore::GetValue(const ConfigKey& key) noexcept {
		if(!HasValue(key))
			return std::nullopt;
		return values.find(key)->second;
	}

	void ConfigStore::SetValue(const ConfigKey& key, const ConfigValue& value) {
		if(!HasValue(key))
			return (void)values.insert({ key, value });

		// Update existing property
		values[key] = value;
	}


}