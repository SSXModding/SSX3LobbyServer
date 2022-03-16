cmake_minimum_required(VERSION 3.22)
project(ssx3lobbyserver)

# Dependencies
find_package(Boost REQUIRED COMPONENTS system program_options)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


add_subdirectory(src/main)