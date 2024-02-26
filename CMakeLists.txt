#
# iniparser library
#
# CMake file used to build library
#
# Copyright 2023      Felipe Montero Bruni (felipx)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the MIT License
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

cmake_minimum_required(VERSION 3.5)

project(iniparser C)

set(PROJECT_DESCRIPTION "Library to parse ini configuration files")
set(PROJECT_HOMEPAGE_URL "https://github.com/felipx/iniparser")
set(LIBRARY_VERSION_MAJOR "1")
set(LIBRARY_VERSION_MINOR "0")
set(LIBRARY_VERSION_PATCH "0")

set(PROJECT_VERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}.${LIBRARY_VERSION_PATCH}")
set(PROJECT_VERSION_MAJOR ${LIBRARY_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${LIBRARY_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${LIBRARY_VERSION_PATCH})

set(LIBRARY_VERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}.${LIBRARY_VERSION_PATCH}")
set(LIBRARY_SOVERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}")

# directories and source
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (NOT MSVC)
    set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wconversion")
endif()

set(LIB_SRC
    ${INC_DIR}/iniparser.h
    ${SRC_DIR}/iniparser.c
)

# Option to build shared library
option(BUILD_SHARED "Build shared library" ON)

# Option to build static library
option(BUILD_STATIC "Build static library" OFF)

# Option to build examples
option(BUILD_EXAMPLE "Build example programs" OFF)

if (NOT BUILD_STATIC AND NOT BUILD_SHARED)
    message(FATAL_ERROR "BUILD_SHARED and BUILD_STATIC cannot be both disabled")
endif ()

if (BUILD_STATIC)
    add_library(iniparser_static STATIC ${LIB_SRC})
    add_library(Iniparser::Iniparser-static ALIAS iniparser_static)
    target_include_directories(iniparser_static
        PUBLIC "$<BUILD_INTERFACE:${INC_DIR}>"
        PUBLIC "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
        PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    target_compile_definitions(iniparser_static PUBLIC O_STATIC_LIBRARY)
    set_target_properties(inipaser_static PROPERTIES
            PUBLIC_HEADER "${INC_DIR}/iniparser.h"
            OUTPUT_NAME iniparser
            EXPORT_NAME Iniparser-static)
    if (MSVC)
        set_target_properties(iniparser_static PROPERTIES OUTPUT_NAME iniparser-static)
    endif ()
    set(iniparser_lib iniparser_static)
endif ()

if (BUILD_SHARED)
    add_library(iniparser SHARED ${LIB_SRC})
    add_library(Iniparser::Iniparser ALIAS iniparser)
    target_include_directories(iniparser
        PUBLIC "$<BUILD_INTERFACE:${INC_DIR}>"
        PUBLIC "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
        PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    set_target_properties(iniparser PROPERTIES
            PUBLIC_HEADER "${INC_DIR}/iniparser.h"
            VERSION "${LIBRARY_VERSION}"
            SOVERSION "${LIBRARY_SOVERSION}"
            WINDOWS_EXPORT_ALL_SYMBOLS TRUE
            EXPORT_NAME Iniparser)
    if (WIN32)
        set_target_properties(iniparser PROPERTIES SUFFIX "-${LIBRARY_VERSION_MAJOR}.dll")
    endif ()
    set(iniparser_lib iniparser)
endif ()

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

message(STATUS "Build shared library:           ${BUILD_SHARED}")
message(STATUS "Build static library:           ${BUILD_STATIC}")
message(STATUS "Build example programs:         ${BUILD_EXAMPLE}")