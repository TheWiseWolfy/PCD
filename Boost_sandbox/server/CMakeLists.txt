project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server)
add_executable(serverMain_noSSL.cpp)

target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE ssl crypto pthread)

target_link_libraries(server_noSLL PRIVATE Threads::Threads)
target_link_libraries(server_noSLL PRIVATE ssl crypto pthread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found. Please set Boost_ROOT to the correct Boost installation path.")
endif()

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(server_noSLL PRIVATE ${Boost_INCLUDE_DIRS})
