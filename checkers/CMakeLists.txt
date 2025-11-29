cmake_minimum_required(VERSION 3.29)
project(checkers2)
include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
find_package( SFML 2.5.1 QUIET COMPONENTS system window graphics )
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.5.1)
FETCHCONTENT_MAKEAVAILABLE(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(checkers2 main.cpp
        game.h
        game.cpp
        board.h
        board.cpp
        piece.h
        piece.cpp
        pawn.h
        pawn.cpp
        king.h
        king.cpp)
target_link_libraries(checkers2 sfml-graphics)