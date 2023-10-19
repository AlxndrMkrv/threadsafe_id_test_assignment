cmake_minimum_required(VERSION 3.15)

project(threadsafe_id_test_assignment LANGUAGES CXX)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(SEND_ERROR "In-source build forbidden")
endif()

add_executable(check Test.cxx ThreadSafeID.hxx ThreadSafeID.cxx)
