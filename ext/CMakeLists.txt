# Libraries
add_subdirectory(libgl3w)

add_library(imgui
    STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
)
target_include_directories(imgui
    PUBLIC
    ${GLFW_INCLUDE_DIR}
    imgui
    imgui/examples/opengl3_example
)
target_link_libraries(imgui
    PRIVATE
    ${OPENGL_LIBRARIES}
    libgl3w
)
