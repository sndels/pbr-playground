cmake_minimum_required(VERSION 3.4)

project(skunkwork)

# Set empty flag for Win32 build
set(wingui)

# Platform specific settings
if (APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_MACOSX_RPATH ON)
    list(APPEND CMAKE_MODULE_PATH "~/cmake_modules")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
    set(CLANG_COVERAGE_LINK_FLAGS  "-framework Cocoa -framework IOKit -framework CoreVideo")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CLANG_COVERAGE_LINK_FLAGS}")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
    set(wingui WIN32)
else()
    message(FATAL_ERROR "Build is only configured for AppleClang and MSVC")
endif()

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

add_subdirectory(ext)

include_directories(include)

# Add full path to res directory
add_definitions(-DRES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/res/")

add_executable(skunkwork WIN32
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(skunkwork
    PRIVATE
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

target_link_libraries(skunkwork
    PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    libgl3w
    imgui
)
