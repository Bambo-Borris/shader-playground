cmake_minimum_required(VERSION 3.16)
project(shader-playground CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive-)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(shader-playground WIN32)
    target_link_libraries(shader-playground PRIVATE SFML::Main)
else()
    add_executable(shader-playground)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_sources(shader-playground PRIVATE src/appicon.rc)
endif()

target_sources(shader-playground PRIVATE
    src/Main.cpp
    src/App.cpp
    src/ShaderManager.cpp
    src/TextureManager.cpp)
target_link_libraries(shader-playground PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML spdlog)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(shader-playground PRIVATE SHADER_PLAYGROUND_DEBUG)
endif()

add_custom_target(format
    COMMAND clang-format -i `git ls-files *.hpp *.cpp`
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_target(run COMMAND shader-playground WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
