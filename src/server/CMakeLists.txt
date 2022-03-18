add_library(ls_server
		AsioImpl.cpp
		BaseMessage.cpp
		MessageReader.cpp
		Server.cpp

		# message files
		messages/PingMessage.cpp
)

set_target_properties(ls_server PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS ON
		CXX_STANDARD_REQUIRED ON
)

target_include_directories(ls_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(ls_server PRIVATE ls::common)
target_link_libraries(ls_server PUBLIC Boost::system fmt::fmt)

# ASIO flags
target_compile_definitions(ls_server PUBLIC
		# No more io_service! No more io_context::work!
		-DBOOST_ASIO_NO_DEPRECATED=1

		-DBOOST_ASIO_DISABLE_BOOST_ARRAY=1
		-DBOOST_ASIO_DISABLE_BOOST_BIND=1
		-DBOOST_ASIO_DISABLE_BOOST_DATE_TIME=1
		-DBOOST_ASIO_DISABLE_BOOST_REGEX=1
		-DBOOST_ASIO_SEPARATE_COMPILATION=1
)


add_library(ls::server ALIAS ls_server)