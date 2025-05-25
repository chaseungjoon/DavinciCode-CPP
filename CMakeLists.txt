cmake_minimum_required(VERSION 3.31)
project(DavincicodeCPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(DavincicodeCPP main.cpp
        Card.h
        Card.cpp
        Player.h
        Player.cpp
        Game.h
        Game.cpp
)
