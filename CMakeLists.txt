cmake_minimum_required(VERSION 4.2)
project(gen_uuid)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

add_executable(gen_uuid
        main.cpp
        help_command.cpp
        reservation_store.cpp
        reserve_command.cpp
        uuid_command.cpp
        uuid_generator.cpp)
