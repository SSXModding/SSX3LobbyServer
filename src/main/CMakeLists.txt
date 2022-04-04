add_executable(ssx3lobbyserver
		Arguments.cpp
		main.cpp

		# Server stuff
		server/AsioImpl.cpp
		server/BaseMessage.cpp
		server/MessageReader.cpp
		server/Server.cpp
		server/Client.cpp

		# TODO: DB

		# message files
		server/messages/PingMessage.cpp
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

target_include_directories(ssx3lobbyserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/server)

target_compile_definitions(ssx3lobbyserver PUBLIC
		-DSPDLOG_FMT_EXTERNAL=1

		# No more io_service! No more io_context::work!
		-DBOOST_ASIO_NO_DEPRECATED=1

		-DBOOST_ASIO_DISABLE_BOOST_ARRAY=1
		-DBOOST_ASIO_DISABLE_BOOST_BIND=1
		-DBOOST_ASIO_DISABLE_BOOST_DATE_TIME=1
		-DBOOST_ASIO_DISABLE_BOOST_REGEX=1
		-DBOOST_ASIO_SEPARATE_COMPILATION=1
		)