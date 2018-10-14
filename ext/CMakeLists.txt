# Pull submodules
execute_process(COMMAND git submodule update --init)

# GLM is header only
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

# Include dependencies as subdirectories when possible
add_subdirectory(libgl3w)
# Override glfw default options
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(glfw)

add_library(imgui STATIC "")
target_sources(imgui
    PRIVATE
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
)
target_include_directories(imgui
    PUBLIC
    glfw/include
    imgui
    imgui/examples/opengl3_example
)
target_link_libraries(imgui
    PUBLIC
    ${OPENGL_LIBRARIES}
    libgl3w
)
