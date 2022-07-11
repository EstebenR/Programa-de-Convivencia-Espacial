cmake_minimum_required(VERSION 2.8.11)
project(PCE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

file(GLOB SOURCE "SourceFiles/*.cpp" "SourceFiles/*.h")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_ttf REQUIRED)
# find_package(Threads REQUIRED)
# if(THREADS_HAVE_PTHREAD_ARG)
#     target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
# endif()
# if(CMAKE_THREAD_LIBS_INIT)
#     target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
# endif()

include_directories(${PROJECT_NAME} 
                                ${SDL2_INCLUDE_DIRS} 
                                ${SDL2_IMAGE_INCLUDE_DIRS} 
                                ${SDL2_TTF_INCLUDE_DIRS} 
                                ${SDL2_NET_INCLUDE_DIRS} 
                                ${SDL2_MIXER_INCLUDE_DIRS}
                                "./Box2D/include/box2d/"
                                "./json/single_include/nlohmann/"
                                "./tileson/include/"
                                )
                                
add_subdirectory(tileson)

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} 
                                ${SDL2_LIBRARIES} 
                                ${SDL2_IMAGE_LIBRARIES}
                                ${SDL2_TTF_LIBRARIES} 
                                ${SDL2_NET_LIBRARIES} 
                                ${SDL2_MIXER_LIBRARIES}
                                ${CMAKE_SOURCE_DIR}/Box2D/lib/libbox2d.a
                                tileson
                                )