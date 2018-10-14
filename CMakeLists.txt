cmake_minimum_required(VERSION 3.4)

project(pbr-playground)

# Set up external dependencies
find_package(OpenGL REQUIRED)

# Set up sub-builds and sources
add_subdirectory(ext)
add_subdirectory(include)
add_subdirectory(src)

# Set absolute path to res directory
add_definitions(-DRES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/res/")

# Set up project targets
# WIN32 tells to not build a cmd-app on windows
add_executable(pbr-playground WIN32
    ${SOURCES}
    ${HEADERS}
)
target_compile_features(pbr-playground
    PRIVATE
    cxx_std_14
)
if (MSVC)
    target_compile_options(pbr-playground
        PRIVATE
        /Wall
    )
else()
    target_compile_options(pbr-playground
        PRIVATE
        -Wall
        -pedantic
    )
endif()
target_include_directories(pbr-playground
    PRIVATE
    ${INCLUDE_DIR}
)
target_link_libraries(pbr-playground
    PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    glm
    libgl3w
    imgui
)
