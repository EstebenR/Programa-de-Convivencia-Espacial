find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
  set(SDL2_MIXER_SDL2_NOT_FOUND "Could NOT find SDL2 (SDL2 is required by SDL2_mixer).")
  if(SDL2_mixer_FIND_REQUIRED)
    message(FATAL_ERROR ${SDL2_MIXER_SDL2_NOT_FOUND})
  else()
      if(NOT SDL2_mixer_FIND_QUIETLY)
        message(STATUS ${SDL2_MIXER_SDL2_NOT_FOUND})
      endif()
    return()
  endif()
  unset(SDL2_MIXER_SDL2_NOT_FOUND)
endif()


# Define options for searching SDL2_mixer Library in a custom path

set(SDL2_MIXER_PATH "" CACHE STRING "Custom SDL2_mixer Library path")

set(_SDL2_MIXER_NO_DEFAULT_PATH OFF)
if(SDL2_MIXER_PATH)
  set(_SDL2_MIXER_NO_DEFAULT_PATH ON)
endif()

set(SDL2_MIXER_NO_DEFAULT_PATH ${_SDL2_MIXER_NO_DEFAULT_PATH}
    CACHE BOOL "Disable search SDL2_mixer Library in default path")
unset(_SDL2_MIXER_NO_DEFAULT_PATH)

set(SDL2_MIXER_NO_DEFAULT_PATH_CMD)
if(SDL2_MIXER_NO_DEFAULT_PATH)
  set(SDL2_MIXER_NO_DEFAULT_PATH_CMD NO_DEFAULT_PATH)
endif()

# Search for the SDL2_mixer include directory
find_path(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h
  HINTS
    ENV SDL2MIXERDIR
    ENV SDL2DIR
    ${SDL2_MIXER_NO_DEFAULT_PATH_CMD}
  PATH_SUFFIXES SDL2
                # path suffixes to search inside ENV{SDL2DIR}
                # and ENV{SDL2MIXERDIR}
                include/SDL2 include
  PATHS ${SDL2_MIXER_PATH}
  DOC "Where the SDL2_mixer headers can be found"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

# Search for the SDL2_mixer library
find_library(SDL2_MIXER_LIBRARY
  NAMES SDL2_mixer
  HINTS
    ENV SDL2MIXERDIR
    ENV SDL2DIR
    ${SDL2_MIXER_NO_DEFAULT_PATH_CMD}
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
  PATHS ${SDL2_MIXER_PATH}
  DOC "Where the SDL2_mixer Library can be found"
)

# Read SDL2_mixer version
if(SDL2_MIXER_INCLUDE_DIR AND EXISTS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MIXER_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MIXER_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_MIXER_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_MAJOR "${SDL2_MIXER_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_MINOR "${SDL2_MIXER_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_PATCH "${SDL2_MIXER_VERSION_PATCH_LINE}")
  set(SDL2_MIXER_VERSION_STRING ${SDL2_MIXER_VERSION_MAJOR}.${SDL2_MIXER_VERSION_MINOR}.${SDL2_MIXER_VERSION_PATCH})
  unset(SDL2_MIXER_VERSION_MAJOR_LINE)
  unset(SDL2_MIXER_VERSION_MINOR_LINE)
  unset(SDL2_MIXER_VERSION_PATCH_LINE)
  unset(SDL2_MIXER_VERSION_MAJOR)
  unset(SDL2_MIXER_VERSION_MINOR)
  unset(SDL2_MIXER_VERSION_PATCH)
endif()

set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_mixer
                                  REQUIRED_VARS SDL2_MIXER_LIBRARIES SDL2_MIXER_INCLUDE_DIRS
                                  VERSION_VAR SDL2_MIXER_VERSION_STRING)


mark_as_advanced(SDL2_MIXER_PATH
                 SDL2_MIXER_NO_DEFAULT_PATH
                 SDL2_MIXER_LIBRARY
                 SDL2_MIXER_INCLUDE_DIR)


if(SDL2_MIXER_FOUND)

  # SDL2::Mixer target
  if(SDL2_MIXER_LIBRARY AND NOT TARGET SDL2::Mixer)
    add_library(SDL2::Mixer UNKNOWN IMPORTED)
    set_target_properties(SDL2::Mixer PROPERTIES
                          IMPORTED_LOCATION "${SDL2_MIXER_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIR}"
                          INTERFACE_LINK_LIBRARIES SDL2::Core)
  endif()
endif()