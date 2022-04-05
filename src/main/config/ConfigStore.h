//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CONFIGSTORE_H
#define SSX3LOBBYSERVER_CONFIGSTORE_H

#include <string>
#include <unordered_map>
#include <variant>
#include <optional>

namespace ls {

	/**
	 * Class which maintains store of configuration in a backend-independent fashion.
	 * This allows multiple backends to interact with the store.
	 */
	struct ConfigStore {
		using ConfigKey = std::string;
		using ConfigValue = std::variant<std::string, int>;

		/**
		 * Get value and typecheck in one smooth operation.
		 */
		template<class T>
		inline std::optional<T> GetValue(const ConfigKey& key) {
			auto opt = GetValue(key);
			if(!opt.has_value())
				return std::nullopt;

			if(auto& var = *opt; std::holds_alternative<T>(var))
				return std::get<T>(var);

			// variant doesn't hold requested type
			return std::nullopt;
		}

		std::optional<ConfigValue> GetValue(const ConfigKey& key);

		void SetValue(const ConfigKey& key, const ConfigValue& value);

		// template inlined version to decay variant construction until configstore?

	   private:
		std::unordered_map<ConfigKey, ConfigValue> values;
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CONFIGSTORE_H
