//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CONFIGSTORE_HPP
#define SSX3LOBBYSERVER_CONFIGSTORE_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace ls {

	// some TODO's:
	// 	- Array proxy object that operator[] returns, so we can use indexing operator for further brevity?

	/**
	 * Class which maintains a store of configuration in a backend-independent fashion,
	 * as a K-V pair.
	 * This allows multiple backends to interact with the store.
	 */
	struct ConfigStore {
		/**
		 * Key type. Exposed for brevity, and to make it easier for
		 * the store functions to be switched out (without pain).
		 */
		using ConfigKey = std::string;

		/**
		 * Value type.
		 */
		using ConfigValue = std::variant<std::string, int>;

		/**
		 * Get value and typecheck in one smooth operation.
		 *
		 * \param[in] key Key to retrieve.
		 * \return Optional with direct type on success; std::nullopt otherwise
		 */
		template <class T>
		inline std::optional<T> GetValue(const ConfigKey& key) noexcept {
			auto opt = GetValue(key);

			// Returned optional doesn't hold a value.
			if(!opt.has_value())
				return std::nullopt;

			// If the optional holds a value, check if it holds the requested type
			if(auto& var = *opt; std::holds_alternative<T>(var))
				return std::get<T>(var);

			// The variant held inside of the optional doesn't hold the requested type,
			// so return nullopt (instead of throwing.).
			return std::nullopt;
		}

		bool HasValue(const ConfigKey& key) const;

		/**
		 * Get value from the store. Base-class version, implemented out of line.
		 *
		 * \param[in] key Key to retrieve.
		 * \return Optional with variant type on success; std::nullopt otherwise
		 */
		std::optional<ConfigValue> GetValue(const ConfigKey& key) noexcept;

		void SetValue(const ConfigKey& key, const ConfigValue& value);

		// template inlined version to decay variant construction until configstore?

	   private:
		/**
		 * Collection of all config values.
		 */
		std::unordered_map<ConfigKey, ConfigValue> values;
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CONFIGSTORE_HPP
