cmake_minimum_required(VERSION 3.5)
project(CheckersQtClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Network Svg REQUIRED)

add_executable(checkers_qt_client
    main.cpp
    loginwindow.h
    loginwindow.cpp
    lobbywindow.h
    lobbywindow.cpp
    gameboardwindow.h
    gameboardwindow.cpp
    movevotingpanel.h
    movevotingpanel.cpp
    networkmanager.h
    networkmanager.cpp
    checkerpiecebutton.h
    checkerpiecebutton.cpp
)

target_link_libraries(checkers_qt_client Qt5::Widgets Qt5::Network Qt5::Svg)

set_target_properties(checkers_qt_client PROPERTIES AUTOMOC ON)
