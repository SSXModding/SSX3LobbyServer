# This might get actual translation units later.
add_library(ls_common INTERFACE
		include/ls/common/Badge.hpp
		include/ls/common/ByteSwap.hpp
		include/ls/common/FixedString.hpp
		include/ls/common/FourCC.hpp
		)

target_include_directories(ls_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# require C++20 to build common code
target_compile_features(ls_common INTERFACE cxx_std_20)

add_library(ls::common ALIAS ls_common)