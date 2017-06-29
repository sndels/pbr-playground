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

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

add_subdirectory(ext)

include_directories(include ext/rocket/lib ext/bass ext/glm ext/glfw/include)

# Add full path to res directory
add_definitions(-DRES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/res/")

# WIN32 tells to not build a cmd-app on windows
add_executable(skunkwork WIN32
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(skunkwork
    PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    libgl3w
    librocket
    imgui
)
if (MSVC)
    target_link_libraries(skunkwork
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/bass/lib/bass.lib
    )
elseif (APPLE)
    target_link_libraries(skunkwork
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/bass/lib/libbass.dylib
    )
else()
    target_link_libraries(skunkwork
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/bass/lib/libbass.so
    )
endif()

# Copy dynamic libraries to build folder after build
# A cleaner way of doing this even with msvc's build structure?
add_custom_command(TARGET skunkwork POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/ext/bass/lib
                           $<TARGET_FILE_DIR:skunkwork>)
