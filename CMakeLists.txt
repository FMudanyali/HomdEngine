# Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.13)
project(HomdEngine)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# Change this if youre using other environments than MSYS2 CLANG64
# or it is in a different path.
SET(win_environment "C:/msys64/clang64")

# Set compiler to clang++
IF(WIN32)
    SET(CMAKE_CXX_COMPILER "${win_environment}/bin/clang++")
ELSE()
    SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
ENDIF()

# Apple M1 does not support -march=native in clang version 14 and before.
IF(APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    SET(arch_flags "-mcpu=apple-m1 -mtune=native")
ELSE()
    SET(arch_flags "-march=native -mtune=native")
ENDIF()

# Whether to optimize or leave debugging information depending on
# build type
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(LINK_FLAGS "${debug_linker}")
    SET(optimize_flags "-Og -g3 -ffunction-sections -fdata-sections")
    ADD_DEFINITIONS(-DDEBUG)
    IF(WIN32)
        SET(win_console "-mconsole")
    ENDIF()
ELSE()
    SET(optimize_flags "-O3 -ffast-math")
ENDIF()

# Needed for ImGui under Windows, also have to rename main() to wmain()
# in main.cpp
IF(WIN32)
    SET(win_flags "-municode")
ENDIF()

# Dead code stripping in the linker part
SET(debug_linker "-Wl,-dead_strip")

# Enable all kinds of warnings and treat them as errors for a better code
# Some warnings are disabled to prevent making changes on dependencies.
SET(warn_flags "\
    -Wall\
    -fno-common\
    -Wextra\
    -Werror\
    -Wshadow\
    -Wformat=2\
    -Wundef\
    -Wno-unused-parameter\
    -Wno-sign-conversion\
    -Wno-conversion\
    -Wno-format-nonliteral\
")

# Put all the flags we have gathered
SET(CMAKE_CXX_FLAGS 
    "${arch_flags} ${optimize_flags} ${warn_flags} ${win_flags} ${win_console}"
)

# The main project files
SET(SOURCE_FILES
    src/game/game.cpp

    src/graphics/graphics.cpp

    src/input/input.cpp

    src/scene/gears/gears.cpp

    src/window/window.cpp

    src/main.cpp
)

# /SDL2 and /opencv4 are for using same imports
# under all environments.
IF(APPLE)
    SET(includes
        # M1 Mac with Homebrew
        "/opt/homebrew/include"
        # Intel Mac with Homebrew / Macports
        "/opt/local/include"
    )
    SET(links
        # M1 Mac with Homebrew
        "/opt/homebrew/lib"
        # Intel Mac with Homebrew / Macports
        "/opt/local/lib"
    )
ENDIF()

INCLUDE_DIRECTORIES(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${includes}
)

LINK_DIRECTORIES(
    ${links}
)

ADD_EXECUTABLE(HomdEngine
    ${SOURCE_FILES}
)

IF(WIN32)
    SET(win32_link -mwindows)
ENDIF()

TARGET_LINK_LIBRARIES(HomdEngine
    sdl2
    opengl32
    glew32
    ${win32_link}
)

# To profile the debug build of the application using Instruments
# under macOS, we need to replace its signature to allow profiling.
IF(APPLE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_CUSTOM_COMMAND(
        TARGET HomdEngine POST_BUILD
        COMMAND /usr/bin/codesign -s -
        --entitlements ${CMAKE_SOURCE_DIR}/debug.entitlements
        -f ${CMAKE_CURRENT_BINARY_DIR}/HomdEngine
    )
ENDIF()