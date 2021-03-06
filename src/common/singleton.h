//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_SINGLETON_H
#define SSX3LOBBYSERVER_SINGLETON_H

namespace ls {

	/**
	 * Singleton which adheres to the "Construct On First Use" idiom.
	 */
	template <class T>
	struct CofuSingleton {

		T& operator()() {
			if(!initalized) {
				initalized = true;
				return *new (&t) T;
			}

			return t;
		}
	   private:
		static T t;
		bool initalized{false};
	};

	template <class T>
	T CofuSingleton<T>::t;
}

#endif // SSX3LOBBYSERVER_SINGLETON_H
