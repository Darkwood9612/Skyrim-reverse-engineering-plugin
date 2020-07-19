cmake_minimum_required(VERSION 3.15)
project(skse64-plugin)

set(DLL_NAME "ReverseHook")

include(cmake_scripts_collection.cmake)

set(third_party "${CMAKE_SOURCE_DIR}/third_party")
set(lib_root "${CMAKE_SOURCE_DIR}/third_party/CommonLibSSE")

file(GLOB_RECURSE lib_src "${lib_root}/src/*" "${lib_root}/include/*")
add_library(CommonLibSSE STATIC ${lib_src})
target_include_directories(CommonLibSSE PUBLIC "${lib_root}/include" "${third_party}")
target_compile_options(CommonLibSSE PUBLIC "/FI\"ForceInclude.h\"" "/FI\"SKSE/Logger.h\"")
target_link_libraries(CommonLibSSE PUBLIC Version)

file(GLOB_RECURSE common_src "${third_party}/common/*")
add_library(common STATIC ${common_src})
target_compile_options(common PRIVATE "/FI\"IPrefix.h\"")
target_include_directories(common PRIVATE "${third_party}/common" "${third_party}")

file(GLOB_RECURSE skse64_common_src "${third_party}/skse64_common/*")
add_library(skse64_common STATIC ${skse64_common_src})
target_compile_options(skse64_common PRIVATE "/FI\"IPrefix.h\"")
target_include_directories(skse64_common PRIVATE "${third_party}/common" "${third_party}/skse64_common" "${third_party}")

file(GLOB_RECURSE skse_src "${third_party}/skse64/*")
add_library(skse64 STATIC ${skse_src})
target_compile_options(skse64 PRIVATE "/FI\"IPrefix.h\"")
target_include_directories(skse64 PUBLIC "${third_party}/common" "${third_party}/skse64" "${third_party}")
target_compile_definitions(skse64 PRIVATE RUNTIME=1 RUNTIME_VERSION=0x01050610)
target_link_libraries(skse64 PUBLIC common skse64_common)

file(GLOB_RECURSE src "src/*")
list(APPEND src "${CMAKE_SOURCE_DIR}/.clang-format")
add_library(skse64_plugin SHARED ${src})
target_link_libraries(skse64_plugin PUBLIC skse64 CommonLibSSE)
set_target_properties(skse64_plugin PROPERTIES OUTPUT_NAME ${DLL_NAME})
target_include_directories(skse64_plugin PRIVATE "${third_party}")
target_link_libraries(skse64_plugin PRIVATE "${third_party}/frida/frida-gum.lib")


apply_default_settings(TARGETS CommonLibSSE common skse64_common skse64 skse64_plugin)
