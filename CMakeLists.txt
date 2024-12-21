cmake_minimum_required(VERSION 3.16)
project(ClubManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native -mtune=native -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

include_directories(${PROJECT_SOURCE_DIR}/include)
set(SOURCES
    src/ClubManager.cc
    src/Events.cc
    src/main.cc
    src/Parser.cc
    src/Table.cc
    src/Time.cc
)

add_executable(ClubManagement ${SOURCES})
target_compile_options(ClubManagement PRIVATE -flto)
