add_library(ls_server
		messages/BaseMessage.cpp

		# message implementations
		messages/PingMessage.cpp
)

set_target_properties(ls_server PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS ON
		CXX_STANDARD_REQUIRED ON
)

target_include_directories(ls_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ls_server PUBLIC Boost::system)

add_library(ls::server ALIAS ls_server)