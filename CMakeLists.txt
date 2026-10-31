cmake_minimum_required(VERSION 3.16)
project(FmCoskunStudioPortfolio VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib QUIET)

# Shared warning flags + raylib linkage for every target
function(fmcoskun_configure_target target)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if (MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if (raylib_FOUND)
        target_link_libraries(${target} PRIVATE raylib)
    elseif (WIN32)
        target_link_libraries(${target} PRIVATE raylib opengl32 gdi32 winmm)
    elseif (APPLE)
        target_link_libraries(${target} PRIVATE raylib "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    else()
        target_link_libraries(${target} PRIVATE raylib GL m pthread dl rt X11)
    endif()
endfunction()

add_executable(fmcoskun_studio main.cpp)
fmcoskun_configure_target(fmcoskun_studio)

# JSON parser tests: ctest (or run json_test <dir-containing-projects.json>)
enable_testing()
add_executable(json_test tests/json_test.cpp)
fmcoskun_configure_target(json_test)
add_test(NAME json_parser COMMAND json_test ${CMAKE_CURRENT_SOURCE_DIR})

# Copy projects.json next to binary on build
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/projects.json ${CMAKE_CURRENT_BINARY_DIR}/projects.json COPYONLY)
