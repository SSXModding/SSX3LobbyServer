//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef SSX3LOBBYSERVER_CLIENT_HPP
#define SSX3LOBBYSERVER_CLIENT_HPP

#include <fmt/core.h>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/compose.hpp>
#include <ls/common/Badge.hpp>
#include <ls/server/message/MessageBase.hpp>
#include <ls/server/message/WireMessageHeader.hpp>
#include <ls/server/Server.hpp>

namespace ls {

	struct Client : public std::enable_shared_from_this<Client> {
		Client(SocketType<tcp> socket, std::shared_ptr<Server> server);

		/**
		 * Called to open the connection.
		 */
		void Open();

		/**
		 * Called to close the connection.
		 */
		void Close();

		/**
		 * Error type returned by AsyncReadMessage().
		 */
		struct ReadError {
			enum class Type {
				Ok, ///< OK. No error
				PartialRead, ///< Partial read; not a fatal error per se?
				InvalidHeader, ///< Invalid header.
				InvalidPayload, ///< Invalid payload
				AsioError ///< Boost.Asio error.
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
				return {
					.type = Type::AsioError,
					.asioError = ec
				};
			}
		};

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
				// Allocate stuff we need to allocate
				messagePayloadBuffer.reserve(512);
				header = std::make_unique<WireMessageHeader>();
			}

			// 1MB max payload size; should be good enough?
			constexpr static auto MAX_PAYLOAD_SIZE = (1024 * 1024);

			SocketType<Protocol>& socket;
			State state;
			std::unique_ptr<WireMessageHeader> header;
			std::vector<std::uint8_t> messagePayloadBuffer;

			std::shared_ptr<MessageBase> messagePtr {};

			void operator()(auto& self, const error_code& error = {}, std::size_t bytesRead = 0) {
				switch(state) {
					case State::ReadHeader:
						state = State::HeaderRead;
						net::async_read(socket, net::buffer(reinterpret_cast<std::uint8_t*>(header.get()), sizeof(WireMessageHeader)), net::transfer_exactly(sizeof(WireMessageHeader)), std::move(self));
						break;

					case State::HeaderRead:
						if(error && error != net::error::operation_aborted) {
							self.complete(ReadError::MakeAsio(error), nullptr);
							return;
						}

						if(bytesRead != sizeof(WireMessageHeader)) {
							self.complete(ReadError::Make(ReadError::Type::PartialRead, "Read {} bytes (when header is {} bytes)", bytesRead, sizeof(WireMessageHeader)), nullptr);
							return;
						}

						if(header->payloadSize > MAX_PAYLOAD_SIZE) {
							self.complete(ReadError::Make(ReadError::Type::InvalidHeader, "Payload size invalid ({} out of max of {})", header->payloadSize, MAX_PAYLOAD_SIZE), nullptr);
							return;
						}

						// Now read the payload
						state = State::ReadPayloadData;
						messagePayloadBuffer.resize(header->payloadSize + 1);
						net::async_read(socket, net::buffer(messagePayloadBuffer), net::transfer_exactly(messagePayloadBuffer.size()), std::move(self));
						break;

					case State::ReadPayloadData:
						if(error && error != net::error::operation_aborted) {
							self.complete(ReadError::MakeAsio(error), nullptr);
							return;
						}

						if(bytesRead != (header->payloadSize + 1)) {
							self.complete(ReadError::Make(ReadError::Type::PartialRead, "Malformed payload (read {} bytes when supposed to read {})", bytesRead, header->payloadSize + 1), nullptr);
							return;
						}

						messagePtr = ls::MessageBase::Create(header->typeCode);

						// Parse properties (if we have to)
						if(header->payloadSize != 1 || header->payloadSize != 0) {
							if(!messagePtr->ReadProperties(messagePayloadBuffer)) {
								messagePayloadBuffer.clear();
								self.complete(ReadError::Make(ReadError::Type::InvalidPayload, "Could not read properties of message"), nullptr);
								return;
							}

							messagePayloadBuffer.clear();
						}

						header.reset();
						self.complete({}, messagePtr);
						return;
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

		template <net::completion_token_for<void(ReadError, std::shared_ptr<MessageBase>)> CompletionToken>
		auto AsyncReadMessage(CompletionToken&& token) {
			using CompletionSig = void(ReadError, std::shared_ptr<MessageBase>);
			return net::async_compose<CompletionToken, CompletionSig>(AsyncReadMessageImpl { socket }, token, socket);
		}

		template <net::completion_token_for<void(error_code)> CompletionToken>
		auto AsyncSendMessage(std::shared_ptr<MessageBase> message, CompletionToken&& token) {
			using CompletionSig = void(error_code);

			// Serialize the message into a buffer used during this operation,
			// which will be cleared once the operation either completes or fails.
			std::vector<std::uint8_t> serialized;
			message->Serialize(serialized);

			return net::async_compose<CompletionToken, CompletionSig>(AsyncSendMessageImpl { socket, std::move(serialized) }, token, socket);
		}

		net::ip::address Address() {
			return socket.remote_endpoint().address();
		}

		std::shared_ptr<Server> GetServer() {
			return server;
		}

	   private:
		Awaitable<void> ReaderCoro();
		SocketType<tcp> socket;

		std::shared_ptr<Server> server;

		// user data:

		// per-IP user data.
		// We use this to
	};

} // namespace ls

#endif // SSX3LOBBYSERVER_CLIENT_HPP
