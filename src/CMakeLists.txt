file(GLOB_RECURSE HEADERS "*.hpp")
file(GLOB_RECURSE SOURCES "*.cpp")
list(REMOVE_ITEM SOURCES "main.cpp")

add_library(externalsort_lib ${HEADERS} ${SOURCES})

add_executable(externalsort "main.cpp")
target_link_libraries(externalsort externalsort_lib)
