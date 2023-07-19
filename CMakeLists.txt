cmake_minimum_required(VERSION 3.24)

project(indishooter,
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic)

#set(INDI_DEBUG 1)
set(INDI_FIND_COMPONENTS client)
include(./cmake/FindINDI.cmake)
#set(CMAKE_MODULE_PATH "./cmake")
#find_package(INDI 2.0 COMPONENTS client REQUIRED)


enable_testing()

add_subdirectory(src/app)
add_subdirectory(src/lib)
