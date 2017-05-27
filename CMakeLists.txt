cmake_minimum_required(VERSION 3.4)

project(skunkwork)

# Platform specific settings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
    if (APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
    endif()
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
    message(FATAL_ERROR "Build is only configured for AppleClang, Clang (Linux) and MSVC")
endif()

find_package(OpenGL REQUIRED)
if (MSVC)
    find_package(GLFW REQUIRED)
else()
    find_package(glfw3 REQUIRED)
endif()
find_package(glm REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

add_subdirectory(ext)

include_directories(include)

# Add full path to res directory
add_definitions(-DRES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/res/")

# WIN32 tells to not build a cmd-app on windows
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
    libgl3w
    imgui
)
if (MSVC)
    target_link_libraries(skunkwork PRIVATE ${GLFW_LIBRARIES})
else()
    target_link_libraries(skunkwork PRIVATE glfw)
endif()
