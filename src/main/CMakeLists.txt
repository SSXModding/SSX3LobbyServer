add_executable(ssx3lobbyserver
		main.cpp

)

set_target_properties(ssx3lobbyserver PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS ON
		CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ssx3lobbyserver PRIVATE ls::server Boost::program_options)