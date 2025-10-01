cmake_minimum_required(VERSION 3.15)
project(OutfitConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCES
    Main.cpp
    Application.cpp
    MemoryEditor.cpp
    FormatConverter.cpp
    FileHandler.cpp
    UIManager.cpp
)

# Header files
set(HEADERS
    OutfitStructures.h
    Application.h
    MemoryEditor.h
    FormatConverter.h
    FileHandler.h
    UIManager.h
)

# Create executable
add_executable(OutfitConverter WIN32 ${SOURCES} ${HEADERS})

# Windows-specific settings
if(WIN32)
    target_compile_definitions(OutfitConverter PRIVATE 
        UNICODE 
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Link Windows libraries
    target_link_libraries(OutfitConverter PRIVATE
        comctl32
        gdi32
        user32
        kernel32
        psapi
    )
    
    # Set subsystem to Windows
    set_target_properties(OutfitConverter PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

# Compiler-specific settings
if(MSVC)
    target_compile_options(OutfitConverter PRIVATE
        /W4          # Warning level 4
        /WX-         # Don't treat warnings as errors
        /MP          # Multi-processor compilation
        /permissive- # Standards conformance
        /Zc:__cplusplus # Enable correct __cplusplus macro
    )
    
    # Set runtime library
    set_property(TARGET OutfitConverter PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    target_compile_options(OutfitConverter PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Debug/Release configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Release optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(OutfitConverter PRIVATE /O2 /Ob2 /Oi /Ot /GL)
        set_target_properties(OutfitConverter PROPERTIES LINK_FLAGS "/LTCG")
    else()
        target_compile_options(OutfitConverter PRIVATE -O3)
    endif()
endif()

# Installation
install(TARGETS OutfitConverter
    RUNTIME DESTINATION bin
)

# Create resource file (optional)
if(WIN32)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/resource.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/resource.rc
        @ONLY
    )
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
