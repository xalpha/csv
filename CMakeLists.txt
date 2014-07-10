################################################################################
#                                                                              #
# This is a simple C++ class for one shot IO with CSV files                    #
#                                                                              #
# Copyright (C) 2014 Alexandru Duliu                                           #
#                                                                              #
# This Source Code Form is subject to the terms of the Mozilla                 #
# Public License v. 2.0. If a copy of the MPL was not distributed              #
# with this file, You can obtain one at http://mozilla.org/MPL/2.0/.           #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 2.8)
project(csv CXX)

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# version
set(CSV_VERSION_MAJOR 0)
set(CSV_VERSION_MINOR 2)
set(CSV_VERSION_PATCH 3)
set(CSV_VERSION ${CSV_VERSION_MAJOR}.${CSV_VERSION_MINOR}.${CSV_VERSION_PATCH})
set(CSV_ABI_VERSION 0.2.3)

# enable C++11 support
if( NOT WIN32 )
    if( CMAKE_COMPILER_IS_GNUCXX )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
    else( CMAKE_COMPILER_IS_GNUCXX )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Qunused-arguments")
    endif()
endif()

# add test app
enable_testing()
include_directories( ${CMAKE_CURRENT_LIST_DIR} )
add_executable( csv_test csv_test.cpp )
add_test( csv_test csv_test )

