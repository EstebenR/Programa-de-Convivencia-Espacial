cmake_minimum_required(VERSION 3.11)
project(tileson)

set(CMAKE_CXX_STANDARD 17)
set(TILESON_PROJECT_VERSION_MAJOR 1)
set(TILESON_PROJECT_VERSION_MINOR 0)
set(TILESON_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTILESON_DEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG 1)
    message(DEBUGGIN')
else()
    set(RELEASE 1)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX 1)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
elseif(APPLE)
    link_directories(/usr/local/opt/llvm/lib/)
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 1)
    set(MSVC 0)
    set(MINGW 0)
elseif(WIN32)
    set(LINUX 0)
    set(WINDOWS 1)
    set(APPLE 0)
    if(MSVC)
        set(MSVC 1)
        set(MINGW 0)
    elseif(MINGW)
        set(MINGW 1)
        set(MSVC 0)
    endif()
else()
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
endif()

option(BUILD_SHARED_LIBS "Build shared libraries." OFF)
option(BUILD_TESTS "Build unit tests." OFF)
if(${APPLE})
    option(USE_CPP17_FILESYSTEM "Uses cpp17 std::filesystem for anything file related" OFF)
else()
    option(USE_CPP17_FILESYSTEM "Uses cpp17 std::filesystem for anything file related" ON)
endif()

if(${USE_CPP17_FILESYSTEM})
    set(TILESON_USE_FILESYSTEM 1)
else()
    set(TILESON_USE_FILESYSTEM 0)
endif()

configure_file (
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h.in"
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h"
)

include_directories(
    ${PROJECT_SOURCE_DIR}/external_libs/
    "../json/single_include/nlohmann/"
    )

set(SOURCE_FILES include/objects/Vector2.hpp include/Tileson.cpp include/Tileson.h include/tiled/Map.cpp include/tiled/Map.h include/tiled/Layer.cpp include/tiled/Layer.h include/tiled/Chunk.cpp include/tiled/Chunk.h include/tiled/Object.cpp include/tiled/Object.h include/objects/Property.cpp include/objects/Property.h include/objects/PropertyCollection.cpp include/objects/PropertyCollection.h include/tiled/Tileset.cpp include/tiled/Tileset.h include/tiled/Tile.cpp include/tiled/Tile.h include/tiled/Frame.cpp include/tiled/Frame.h include/tiled/Terrain.cpp include/tiled/Terrain.h include/tiled/WangSet.cpp include/tiled/WangSet.h include/tiled/WangColor.cpp include/tiled/WangColor.h include/tiled/WangTile.cpp include/tiled/WangTile.h include/tiled/Text.hpp include/tiled/Grid.cpp include/tiled/Grid.h include/objects/Color.hpp include/misc/MemoryBuffer.cpp include/misc/MemoryBuffer.h include/misc/MemoryStream.cpp include/misc/MemoryStream.h)
add_library(tileson ${SOURCE_FILES})

if(${BUILD_TESTS})
    add_subdirectory(tests)
endif()

unset(BUILD_SHARED_LIBS CACHE) #Must clear the cache to detect changes in this variable
unset(BUILD_TESTS CACHE) #Must clear the cache to detect changes in this variable
unset(USE_CPP17_FILESYSTEM CACHE)