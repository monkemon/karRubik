cmake_minimum_required(VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g ")

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

project(${ProjectId} CXX)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_compile_options(-Wall -Wextra -pedantic )

file(GLOB project_sources src/*.cpp)
file(GLOB project_headers src/*.h src/*.hpp)

add_executable(${ProjectId} ${project_sources} ${project_headers})

target_compile_options(${ProjectId} INTERFACE -fsanitize=address)
target_link_libraries(${ProjectId} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
