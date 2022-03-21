add_executable(ssx3lobbyserver
		Arguments.cpp
		main.cpp
)

set_target_properties(ssx3lobbyserver PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS ON
		CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ssx3lobbyserver PRIVATE ls::common
		# TODO: this is laughably platform specific, and we can probably find a better way.
		# Maybe a InitMessages() in ls:: which then calls private initalizer functions?
		"-Wl,--whole-archive" ls::server "-Wl,--no-whole-archive"
		Boost::program_options)