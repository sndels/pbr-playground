cmake_minimum_required(VERSION 3.4)

project(skunkwork)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

add_subdirectory(ext)

include_directories(include)

# Add full path to res directory
add_definitions(-DRES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/res/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")

add_executable(skunkwork WIN32
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(skunkwork
    PRIVATE
    ${GLFW_INCLUDE_DIR}
)

target_link_libraries(skunkwork
    PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    libgl3w
)
