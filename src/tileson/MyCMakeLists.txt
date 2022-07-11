cmake_minimum_required(VERSION 2.8.11)
project(Tileson)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE ./include/*/*.cpp ./include/*/*.h ./include/*/*.hpp)

include_directories(${PROJECT_NAME}
                                "./include/tiled/"
                                "./include/objects/"
                                "./include/misc/"
                                "./include/"
                                "../json/single_include/nlohmann/"
                                )

add_library(${PROJECT_NAME} ${SOURCE})