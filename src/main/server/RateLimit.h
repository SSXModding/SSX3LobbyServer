//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_RATELIMIT_H
#define SSX3LOBBYSERVER_RATELIMIT_H

#include <cstdint>
#include <chrono>

namespace ls {
	/**
	 * Basic rate limiter thing.
	 */
	template <class Dur = std::chrono::seconds, class Clock = std::chrono::steady_clock>
	struct RateLimit {
		/**
		 * Constructor.
		 *
		 * cooldown_time and max_rate are used as Dur, and probably SHOULD be Duration<Dur> or something but I'm lazy atm
		 */
		RateLimit(std::uint32_t max_events, std::uint32_t max_rate, std::uint32_t cooldown_time)
			: cooldown_time(cooldown_time),
			  max_rate(max_rate),
			  max_event_count(max_events) {
		}

		/**
		 * Try and take a single event.
		 *
		 * \returns True if the event was taken. false if the ratelimit was either activated or active.
		 */
		bool TryTakeEvent() {
			auto now = std::chrono::time_point_cast<std::chrono::seconds>(Clock::now());

			if(status == Status::CoolingDown) {
				// If we have gone at or past the cooldown time then we can say we're ready.
				if((now - last_event).count() >= cooldown_time && status == Status::CoolingDown)
					status = Status::NotActive;
				else
					// We have not met or surpassed the cooldown time yet.
					return false;
			}

			if((now - last_event).count() < max_rate) {
				if(++event_count >= max_event_count) {
					// tripped the ratelimit
					status = Status::CoolingDown;
					last_event = now;
					return false;
				}
			} else {
				// The event happened far after we'd care,
				// so reset the event count.
				event_count = 0;
				last_event = now;
			}

			// we can take this event.
			return true;
		}

		/**
		 * Returns true if the ratelimit is active
		 */
		bool IsCoolingDown() const {
			return status == Status::CoolingDown;
		}

	   private:
		enum class Status : std::uint8_t {
			NotActive,
			CoolingDown
		} status{Status::NotActive};

		// initalized in the constructor
		std::uint32_t cooldown_time;

		/**
		 * Max rate in Dur.
		 */
		std::uint32_t max_rate;

		/**
		 * Max event count.
		 */
		std::uint32_t max_event_count;

		/**
		 * Current event count.
		 */
		std::uint32_t event_count { 0 };

		/**
		 * The time point when the last event was (attempted) to be taken.
		 */
		std::chrono::time_point<Clock, Dur> last_event {};
	};

} // namespace ls
#endif
