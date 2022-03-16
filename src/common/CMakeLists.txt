add_library(ls_common INTERFACE)
target_include_directories(ls_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ls::common ALIAS ls_common)