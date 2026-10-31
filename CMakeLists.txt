cmake_minimum_required(VERSION 3.16)
project(FmCoskunStudioPortfolio VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib QUIET)

add_executable(fmcoskun_studio
    fmcoskun_studio_3b_c_masa_st_portfolyo_uygulamas.cpp
)

if (MSVC)
    target_compile_options(fmcoskun_studio PRIVATE /W4)
else()
    target_compile_options(fmcoskun_studio PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (raylib_FOUND)
    target_link_libraries(fmcoskun_studio PRIVATE raylib)
else()
    if (WIN32)
        target_link_libraries(fmcoskun_studio PRIVATE raylib opengl32 gdi32 winmm)
    elseif (APPLE)
        target_link_libraries(fmcoskun_studio PRIVATE raylib "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    else()
        target_link_libraries(fmcoskun_studio PRIVATE raylib GL m pthread dl rt X11)
    endif()
endif()

# Copy projects.json next to binary on build
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/projects.json ${CMAKE_CURRENT_BINARY_DIR}/projects.json COPYONLY)

