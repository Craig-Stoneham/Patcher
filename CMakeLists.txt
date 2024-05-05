cmake_minimum_required(VERSION 3.8)

################################################################

# Extract version information from packer/version.h
file(STRINGS "source/patcher/version.h" VERSION_HEADER_CONTENT REGEX "#define VERSION_(MAJOR|MINOR|PATCH) [0-9]+")

# Extract the version numbers from the content
string(REGEX MATCH "#define VERSION_MAJOR ([0-9]+)" _ ${VERSION_HEADER_CONTENT})
set(VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "#define VERSION_MINOR ([0-9]+)" _ ${VERSION_HEADER_CONTENT})
set(VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "#define VERSION_PATCH ([0-9]+)" _ ${VERSION_HEADER_CONTENT})
set(VERSION_PATCH ${CMAKE_MATCH_1})

# Create a string representation of the version in the format "MAJOR.MINOR.PATCH"
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("patcher Version: ${VERSION_STRING}")


################################################################

project(patcher)

################################################################

# Option: CMake install prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Default install prefix" FORCE)

# Option: Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)

#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/$<CONFIG>)

# Set the directory for the binaries
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/$<CONFIG>)

################################################################

set(MODULE_NAME "my_module" CACHE STRING "The name of the module")
set(SOLUTION_NAME "godot" CACHE INTERNAL "The name of the the godot solution")

################################################################

if(MODULE_NAME STREQUAL "")
  message(FATAL_ERROR "MODULE_NAME is not a valid string")
endif()

if(SOLUTION_NAME STREQUAL "")
  message(FATAL_ERROR "SOLUTION_NAME is not a valid string")
endif()

################################################################

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/patcher")
set(PUGIXML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/vendor/pugixml")

# Source
file(GLOB_RECURSE patcher_HEADERS ${SOURCE_DIR}/*.h)
file(GLOB_RECURSE patcher_SOURCES ${SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE PUGIXML_HEADERS ${PUGIXML_DIR}/*.hpp)
file(GLOB_RECURSE PUGIXML_SOURCES ${PUGIXML_DIR}/*.cpp)

# Project
add_executable(patcher ${ENGINE_LIBRARY_TYPE} ${patcher_HEADERS} ${patcher_SOURCES} ${PUGIXML_HEADERS} ${PUGIXML_SOURCES})

#
target_compile_definitions(patcher PUBLIC MODULE_NAME="${MODULE_NAME}")
target_compile_definitions(patcher PUBLIC SOLUTION_NAME="${SOLUTION_NAME}")

if(${REVERSE_ORDER})
    target_compile_definitions(patcher PRIVATE REVERSE_ORDER_ENABLED)
endif()

# Set the output directory
set_target_properties(patcher PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
  LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
  RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# C++ version
target_compile_features(patcher PRIVATE cxx_std_20)

# Include directory
target_include_directories(patcher PUBLIC ${SOURCE_DIR})
target_include_directories(patcher PUBLIC ${PUGIXML_DIR})

# Group source and header files in IDE

source_group("headers/patcher" FILES ${patcher_HEADERS})
source_group("source/patcher" FILES ${patcher_SOURCES})

source_group("headers/vendor/pugixml" FILES ${PUGIXML_HEADERS})
source_group("source/vendor/pugixml" FILES ${PUGIXML_SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT patcher)
