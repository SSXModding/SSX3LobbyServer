//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_WITHTIMEOUT_HPP
#define SSX3LOBBYSERVER_WITHTIMEOUT_HPP

#include <fmt/core.h>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/experimental/parallel_group.hpp>
#include <ls/asio/AsioConfig.hpp>

namespace ls::detail {

	template <typename CompletionToken>
	struct TimedToken {
		SteadyTimerType& timer;
		CompletionToken& token;
	};

	template <typename... Signatures>
	struct TimedInitiation {
		template <typename CompletionHandler, typename Initiation, typename... InitArgs>
		void operator()(CompletionHandler handler, SteadyTimerType& timer, Initiation&& initiation, InitArgs&&... init_args) {
			using net::experimental::make_parallel_group;

			auto ex = net::get_associated_executor(handler, net::get_associated_executor(initiation));

			// clang-format off
			make_parallel_group(
				net::bind_executor(ex, [&](auto&& token) {
									return timer.async_wait(std::forward<decltype(token)>(token));
							   }),
				net::bind_executor(ex, [&](auto&& token) {
									return net::async_initiate<decltype(token), Signatures...>(std::forward<Initiation>(initiation), token, std::forward<InitArgs>(init_args)...);
							   }))
			.async_wait(net::experimental::wait_for_one(),
				[handler = std::move(handler)](std::array<std::size_t, 2>, std::error_code, auto... underlying_op_results) mutable {
					std::move(handler)(std::move(underlying_op_results)...);
				});
			// clang-format on
		}
	};
} // namespace ls::detail

namespace boost::asio {
	template <typename InnerCompletionToken, typename... Signatures>
	struct async_result<ls::detail::TimedToken<InnerCompletionToken>, Signatures...> {
		template <typename Initiation, typename... InitArgs>
		static auto initiate(Initiation&& init, ls::detail::TimedToken<InnerCompletionToken> t, InitArgs&&... init_args) {
			return asio::async_initiate<InnerCompletionToken, Signatures...>(ls::detail::TimedInitiation<Signatures...> {}, t.token, t.timer, std::forward<Initiation>(init), std::forward<InitArgs>(init_args)...);
		}
	};
} // namespace boost::asio

namespace ls {
	template <typename CompletionToken>
	detail::TimedToken<CompletionToken> Timed(SteadyTimerType& timer, CompletionToken&& token) {
		return detail::TimedToken<CompletionToken> { timer, token };
	}

	/**
	 * Call a ASIO operation with a timeout.
	 * The timer is supplied by the user, and must last until the completion token is called.
	 *
	 * \param[in] op The asynchronous operation to wrap with a timeout
	 * \param[in] timer The timer to use as the deadline timer. Must be configured with a expiry beforehand
	 * \param[in] token The completion token for this operation.
	 */
	template <typename Op, typename CompletionToken>
	auto WithTimeout(Op op, SteadyTimerType& timer, CompletionToken&& token) {
		return std::move(op)(Timed(timer, std::forward<CompletionToken>(token)));
	}
} // namespace ls

#endif // SSX3LOBBYSERVER_WITHTIMEOUT_HPP
