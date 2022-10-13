add_executable(ssx3lobbyserver

		# Configuration code
		src/config/ConfigStore.cpp
		src/config/backends/CommandLineConfigBackend.cpp

		# Server code
		src/asio/AsioImpl.cpp
		src/Server.cpp
		src/Client.cpp

		# message classes/handlers
		src/message/MessageBase.cpp
		src/message/messages/PingMessage.cpp

		# TODO: DB/model

		# main entry
		src/main.cpp
)


target_link_libraries(ssx3lobbyserver PRIVATE
		ls::common
		Boost::system
		fmt::fmt
		spdlog::spdlog
		Boost::program_options
)

target_include_directories(ssx3lobbyserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(ssx3lobbyserver PUBLIC
		-DSPDLOG_FMT_EXTERNAL=1

		# No more io_service! No more io_context::work!
		-DBOOST_ASIO_NO_DEPRECATED=1

		-DBOOST_ASIO_DISABLE_BOOST_ARRAY=1
		-DBOOST_ASIO_DISABLE_BOOST_BIND=1
		-DBOOST_ASIO_DISABLE_BOOST_DATE_TIME=1
		-DBOOST_ASIO_DISABLE_BOOST_REGEX=1
		-DBOOST_ASIO_SEPARATE_COMPILATION=1

		# Enable io_uring support on Linux
		-DBOOST_ASIO_DISABLE_EPOLL=1
		)