cmake_minimum_required(VERSION 3.10)

project(broom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Threads REQUIRED)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Werror -O2")

set(EXECUTABLE_OUTPUT_PATH ../bin)

add_executable(broom ../src/main.cpp ../src/entry.cpp ../src/broom.cpp ../src/group.hpp)
target_link_libraries(broom Threads::Threads)
