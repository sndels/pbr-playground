# Libraries
add_subdirectory(libgl3w)
add_subdirectory(glfw)

add_library(imgui
    STATIC
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
    PRIVATE
    ${OPENGL_LIBRARIES}
    libgl3w
)

add_library(librocket
    STATIC
    rocket/lib/device.c
    rocket/lib/track.c
)
if(MSVC)
    target_link_libraries(librocket
        PRIVATE
        ws2_32.lib
    )
endif()
