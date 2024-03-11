project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client clientMain.cpp)

target_link_libraries(client PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE ssl crypto pthread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found. Please set Boost_ROOT to the correct Boost installation path.")
endif()

target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})


