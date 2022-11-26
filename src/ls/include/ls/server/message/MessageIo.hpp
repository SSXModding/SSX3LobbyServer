//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_MESSAGEIO_HPP
#define SSX3LOBBYSERVER_MESSAGEIO_HPP

#include <fmt/core.h>

#include <boost/asio/compose.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/write.hpp>
#include <ls/asio/AsioConfig.hpp>
#include <ls/asio/WithTimeout.hpp>
#include <ls/server/message/MessageBase.hpp>
#include <ls/server/message/WireMessageHeader.hpp>

namespace ls {

	/**
	 * Error type returned by AsyncReadMessage().
	 */
	struct ReadError {
		enum class Type {
			Ok,				///< OK. No error
			PartialRead,	///< Partial read; not a fatal error per se?
			InvalidHeader,	///< Invalid header.
			InvalidPayload, ///< Invalid payload
			AsioError		///< Boost.Asio error.
		};

		Type type { Type::Ok };

		std::string what;
		error_code asioError;

		constexpr explicit operator Type() const {
			return type;
		}

		constexpr operator bool() const {
			return type == Type::Ok;
		}

		template <class... Args>
		static ReadError Make(Type type, fmt::format_string<Args...> formatString, Args... args) {
			return {
				.type = type,
				.what = fmt::format(formatString, std::forward<Args>(args)...)
			};
		}

		static ReadError MakeAsio(const error_code& ec) {
			fmt::print("asio error: {}\n", ec.message());
			return {
				.type = Type::AsioError,
				.asioError = ec
			};
		}
	};

	namespace detail {

		// TODO: Reading should take advantage of the Asio recycling allocator.

		template <typename Protocol>
		struct AsyncReadMessageImpl {
			enum class State {
				ReadHeader,		///< Start; read the header
				HeaderRead,		///< Header read; whether successful or not
				ReadPayloadData ///< Payload data read; whether successful or not.
			};

			explicit constexpr AsyncReadMessageImpl(SocketType<Protocol>& socket)
				: socket(socket),
				  state(State::ReadHeader) {
			}

			// 1MB max payload size; should be good enough?
			constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024);

			SocketType<Protocol>& socket;
			State state;

			std::shared_ptr<SteadyTimerType> deadlineTimer;
			std::shared_ptr<WireMessageHeader> header;
			std::vector<std::uint8_t> messagePayloadBuffer;

			std::shared_ptr<MessageBase> messagePtr {};

			constexpr static auto READ_TIMEOUT = std::chrono::seconds(5);

			void Deallocate() {
				deadlineTimer.reset();
				header.reset();
				messagePayloadBuffer.clear();
			}

			void operator()(auto& self, const error_code& error = {}, std::size_t bytesRead = 0) {
				switch(state) {
					case State::ReadHeader:
						state = State::HeaderRead;

						// Allocate objects used throughout the operation.
						messagePayloadBuffer.reserve(512);
						header = std::allocate_shared<WireMessageHeader>(net::get_associated_allocator(self));
						deadlineTimer = std::allocate_shared<SteadyTimerType>(net::get_associated_allocator(self), socket.get_executor(), READ_TIMEOUT);

						WithTimeout(net::async_read(socket, net::buffer(reinterpret_cast<std::uint8_t*>(header.get()), sizeof(WireMessageHeader)), net::deferred), *deadlineTimer, std::move(self));
						break;

					case State::HeaderRead:
						// Most likely a true EOF, from disconnection.
						if(error) {
							Deallocate();
							fmt::print("Error 1: {}\n", error.message());
							self.complete(error, { });
							break;
						}

						if(bytesRead != sizeof(WireMessageHeader)) {
							Deallocate();
							self.complete(net::error::make_error_code(net::error::misc_errors::eof), nullptr);
							break;
						}

						if(header->payloadSize > MAX_PAYLOAD_SIZE) {
							Deallocate();
							self.complete(net::error::make_error_code(net::error::misc_errors::eof), nullptr);
							break;
						}

						// Now read the payload
						state = State::ReadPayloadData;
						messagePayloadBuffer.resize(header->payloadSize + 1);
						WithTimeout(net::async_read(socket, net::buffer(messagePayloadBuffer), net::deferred), *deadlineTimer, std::move(self));
						break;

					case State::ReadPayloadData:

						if(error) {
							Deallocate();
							fmt::print("Error 2: {}\n", error.message());
							self.complete(error, nullptr);
							break;
						}

						if(bytesRead != (header->payloadSize + 1)) {
							Deallocate();
							self.complete(net::error::make_error_code(net::error::misc_errors::eof), nullptr);
							break;
						}

						messagePtr = ls::MessageBase::Create(header->typeCode);

						// Parse properties (if we have to)
						if(header->payloadSize != 1 || header->payloadSize != 0) {
							if(!messagePtr->ReadProperties(messagePayloadBuffer)) {
								Deallocate();
								self.complete(net::error::make_error_code(net::error::misc_errors::eof), nullptr);
								break;
							}
						}

						Deallocate();
						self.complete({}, messagePtr);
						break;
				}
			}
		};

		template <typename Protocol>
		struct AsyncSendMessageImpl {
			enum class State {
				Start,	///< Start; begin the write
				Written ///< Write has ended, either successfully or not.
			};

			explicit constexpr AsyncSendMessageImpl(SocketType<Protocol>& socket, std::vector<std::uint8_t>&& message)
				: socket(socket),
				  serializedMessage(std::move(message)),
				  state(State::Start) {
			}

			SocketType<Protocol>& socket;
			std::vector<std::uint8_t> serializedMessage;
			State state;

			void operator()(auto& self, const error_code& error = {}, std::size_t bytesSent = 0) {
				switch(state) {
					case State::Started:
						state = State::Written;
						net::async_write(socket, net::buffer(serializedMessage), net::transfer_exactly(serializedMessage.size()), std::move(self));
						break;
					case State::Written:
						serializedMessage.clear();
						self.complete(error);
						return;
				}
			}
		};
	} // namespace detail

	/**
	 * Read a Dirtysock message asynchronously.
	 */
	template <net::completion_token_for<void(error_code, std::shared_ptr<MessageBase>)> CompletionToken, class Protocol>
	auto AsyncReadMessage(SocketType<Protocol>& socket, CompletionToken&& token) {
		using CompletionSig = void(error_code, std::shared_ptr<MessageBase>);
		return net::async_compose<CompletionToken, CompletionSig>(detail::AsyncReadMessageImpl { socket }, token, socket);
	}

	/**
	 * Write a Dirtysock message asynchronously.
	 */
	template <net::completion_token_for<void(error_code)> CompletionToken, class Protocol>
	auto AsyncSendMessage(SocketType<Protocol>& socket, std::shared_ptr<MessageBase> message, CompletionToken&& token) {
		using CompletionSig = void(error_code);

		// Serialize the message into a buffer used during this operation,
		// which will be cleared once the operation either completes or fails.
		std::vector<std::uint8_t> serialized;
		message->Serialize(serialized);

		return net::async_compose<CompletionToken, CompletionSig>(detail::AsyncSendMessageImpl { socket, std::move(serialized) }, token, socket);
	}

} // namespace ls

#endif // SSX3LOBBYSERVER_MESSAGEIO_HPP
