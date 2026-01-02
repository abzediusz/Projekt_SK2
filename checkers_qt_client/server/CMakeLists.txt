cmake_minimum_required(VERSION 3.5)
project(DummyCheckersServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Network REQUIRED)

add_executable(dummy_server dummy_server.cpp dummy_server.h)
target_link_libraries(dummy_server Qt5::Network)

set_target_properties(dummy_server PROPERTIES AUTOMOC ON)
