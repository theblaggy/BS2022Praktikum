cmake_minimum_required(VERSION 3.21)
project(BS2022Praktikum C)

set(CMAKE_C_STANDARD 99)

add_executable(BS2022Praktikum main.c main.h keyValueStore.c keyValueStore.h sub.c sub.h socket.c socket.h commandParser.c commandParser.h)
