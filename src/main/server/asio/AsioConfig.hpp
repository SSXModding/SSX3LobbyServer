//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_ASIOCONFIG_HPP
#define SSX3LOBBYSERVER_ASIOCONFIG_HPP

#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <boost/asio/strand.hpp>
#include <boost/asio/system_executor.hpp>

namespace ls {

	namespace net = boost::asio;
	using tcp = net::ip::tcp;
	using unix_stream = net::local::stream_protocol;

	/**
	 * The type for executors to follow.
	 * This is the basic executor type.
	 */
	using BaseExecutorType =
#ifdef LS_USE_SYSTEM_EXECUTOR
		net::strand<net::system_executor>;
#else
		net::strand<net::io_context::executor_type>;
#endif

	/**
	 * The (real) type executors follow.
	 */
	using ExecutorType = BaseExecutorType; // ideally should use some default, but can't nest as_tuple_t doing it this way.

	/**
	 * CompletionToken for allowing usage of coroutines w/o exception handling.
	 * \see Boost.Asio CompletionToken
	 */
	constexpr inline auto use_tuple_awaitable = net::as_tuple(net::use_awaitable_t<ExecutorType>{});

	/**
	 * Awaitable type (configured for the current executor)
	 */
	template <class T>
	using Awaitable = net::awaitable<T, ExecutorType>;

	template <typename Protocol>
	using SocketType = net::basic_stream_socket<Protocol, ExecutorType>;

//	using TcpSocketType = SocketType<tcp>;
//	using UnixStreamSocketType = SocketType<net::local::stream_protocol>;

	using TimerType = net::basic_waitable_timer<std::chrono::steady_clock, net::wait_traits<std::chrono::steady_clock>, ExecutorType>;

	template <typename Protocol>
	using AcceptorType = net::basic_socket_acceptor<Protocol, ExecutorType>;

} // namespace collab3::core

#endif // SSX3LOBBYSERVER_ASIOCONFIG_HPP
