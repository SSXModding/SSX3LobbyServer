//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_BADGE_HPP
#define SSX3LOBBYSERVER_BADGE_HPP

namespace ls::common {

	/**
	 * A badge. Can only be created by the specified class, or classes defined
	 * inside of that class.
	 *
	 * This allows for gating functions to a specific class, say
	 *
	 * \code
	 * class MySuperSecret {
	 *   public:
	 *      void MyFunc(Badge<SomethingSensible>, ...);
	 * };
	 *
	 * class SomethingSensible {
	 *  public:
	 *     SomethingSensible() {
	 *     		GetSecret().MyFunc({}, ...); // only SomethingSensible can create a Badge<SomethingSensible>
	 *     }
	 * }
	 * \endcode
	 *
	 * Unfortunately, it doesn't seem possible to pass a pack expression to a friend declaration,
	 * which kinda sucks.
	 */
	template <typename T>
	struct Badge {
	   private:
		friend T;
		Badge() = default;
	};

} // namespace ls::common

#endif // SSX3LOBBYSERVER_BADGE_HPP
