cmake_minimum_required(VERSION 3.20.2)
project(Boost_sandbox)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)


set(BOOST_ROOT "libs/boost_1_82_0")
find_package(Boost REQUIRED)

add_subdirectory(client)
add_subdirectory(server)
