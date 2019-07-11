cmake_minimum_required(VERSION "")
project(externalsort)

# Enable C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(googletest)

enable_testing()
add_subdirectory(test)
