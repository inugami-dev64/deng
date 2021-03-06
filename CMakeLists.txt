# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng LANGUAGES CXX C)

# Constant config values
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build DENG testing applications" ON)


# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W4 $<$<COMPILE_LANGUAGE:CXX>:/std:c++17> $<$<COMPILE_LANGUAGE:C>:/std:c17> /wd4251)
	add_compile_definitions(_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH _CRT_SECURE_NO_WARNINGS)

    # Set correct optimisation flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -msse4.1 -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:C>:-std=c99>)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()
endif()


# Check if debugging definition should be considered
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()


# Libnwin configuration
set(NEKOWIN_BUILD_STATIC_LIB ON CACHE BOOL "Build static nekowin library")
set(NEKOWIN_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared nekowin library")
set(NEKOWIN_BUILD_GLAPP OFF CACHE BOOL "Build glapp application")

# Libdas and dastool configuration
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas tests")
set(LIBDAS_BUILD_STATIC_LIB ON CACHE BOOL "Build static libdas library")
set(LIBDAS_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared libdas library")
set(LIBDAS_BUILD_DASTOOL ON CACHE BOOL "Build dastool application")
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas testing applications")

include(cmake/copy.cmake)
include(cmake/FetchTrunk.cmake)
include(cmake/libdeng.cmake)
include(cmake/GameEditor.cmake)
include(cmake/TextureDump.cmake)


# Non-experimental tests
if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/TriangleApp.cmake)
    include(cmake/tests/VulkanImGui.cmake)
    include(cmake/tests/OpenGLImGui.cmake)
    include(cmake/tests/ShaderGenerator.cmake)
    include(cmake/tests/FilePickerTest.cmake)
    include(cmake/tests/MultiFramebuffer.cmake)
    include(cmake/tests/Grid3DTest.cmake)

    if(EXPERIMENTAL_BUILD_CUBIC_SPLINE_TESTS)
        include(cmake/tests/CubicsInterp.cmake)
    endif()

    if(EXPERIMENTAL_BUILD_MATRIX_TESTS)
        include(cmake/tests/MatrixTest.cmake)
    endif()
endif()
