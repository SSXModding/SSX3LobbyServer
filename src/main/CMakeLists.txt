add_executable(ssx3lobbyserver

		# Configuration code
		config/ConfigStore.cpp
		config/backends/CommandLineConfigBackend.cpp

		# Server code
		server/asio/AsioImpl.cpp
		server/MessageBase.cpp
		server/MessageReader.cpp
		server/Server.cpp
		server/Client.cpp

		# TODO: DB/model

		# message classes/handlers
		server/messages/PingMessage.cpp

		# main entry
		main.cpp
)

set_target_properties(ssx3lobbyserver PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS ON
		CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ssx3lobbyserver PRIVATE
		ls::common
		Boost::system
		fmt::fmt
		spdlog::spdlog
		Boost::program_options
)

target_include_directories(ssx3lobbyserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# FIXME: remove
target_include_directories(ssx3lobbyserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/server)

target_compile_definitions(ssx3lobbyserver PUBLIC
		# Comment if this causes a problem on your system
		-DLS_USE_SYSTEM_EXECUTOR

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