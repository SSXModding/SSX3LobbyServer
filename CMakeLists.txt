cmake_minimum_required(VERSION 3.22)
project(ssx3lobbyserver
		VERSION 0.1.0
		LANGUAGES C CXX
		DESCRIPTION "Implementation of a Dirtysock lobby server for SSX 3 online"
		)

# Dependencies
find_package(Boost REQUIRED COMPONENTS system program_options)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src/common)
add_subdirectory(src/ls)