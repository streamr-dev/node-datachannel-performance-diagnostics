cmake_minimum_required(VERSION 3.7)
if(POLICY CMP0079)
	cmake_policy(SET CMP0079 NEW)
endif()

if(WIN32)
	add_executable(multi-node-benchmark main.cpp peer.cpp testclient.cpp parse_cl.cpp parse_cl.h getopt.cpp getopt.h peer.h testclient.h helpers.h)
	target_compile_definitions(multi-node-benchmark PUBLIC STATIC_GETOPT)
else()
	add_executable(multi-node-benchmark main.cpp peer.cpp testclient.cpp parse_cl.cpp parse_cl.h peer.h testclient.h helpers.h)
endif()

set_target_properties(multi-node-benchmark PROPERTIES
	CXX_STANDARD 17
	OUTPUT_NAME multi-node-benchmark)
target_link_libraries(multi-node-benchmark datachannel nlohmann_json)

if(WIN32)
	add_custom_command(TARGET multi-node-benchmark POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<TARGET_FILE_DIR:datachannel>/datachannel.dll"
		$<TARGET_FILE_DIR:multi-node-benchmark>
	)
endif()
