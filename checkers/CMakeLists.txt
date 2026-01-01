cmake_minimum_required(VERSION 3.10.0)
project(checkers VERSION 0.1.0 LANGUAGES C CXX)
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)
add_executable(checkers board.cpp game.cpp king.cpp main.cpp pawn.cpp piece.cpp board.h game.h king.h pawn.h piece.h)
target_compile_features(checkers PRIVATE cxx_std_17)
target_link_libraries(checkers PRIVATE SFML::Graphics)
