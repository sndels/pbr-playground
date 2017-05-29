#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // _WIN32

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <iostream>
#include <sstream>
#include <sync.h>

#include "audioStream.hpp"
#include "logger.hpp"
#include "quad.hpp"
#include "shaderProgram.hpp"
#include "texture.hpp"
#include "timer.hpp"

using std::cout;
using std::cerr;
using std::endl;

namespace {
    const static char* WINDOW_TITLE = "skunkwork";
    GLsizei XRES = 1280;
    GLsizei YRES = 720;
    float LOGW = 600.f;
    float LOGH = 200.f;
    float LOGM = 10.f;
    glm::vec2 CURSOR_POS(0,0);
}

//Set up audio callbacks for rocket
static struct sync_cb audioSync = {
    AudioStream::pauseStream,
    AudioStream::setStreamRow,
    AudioStream::isStreamPlaying
};

void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action,
                 int32_t mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        CURSOR_POS = glm::vec2(2 * xpos / XRES - 1.0, 2 * (YRES - ypos) / YRES - 1.0);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (ImGui::IsMouseHoveringAnyWindow()) {
        ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        CURSOR_POS = glm::vec2(2 * xpos / XRES - 1.0, 2 * (YRES - ypos) / YRES - 1.0);
    }
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    (void) window;
    XRES = width;
    YRES = height;
    glViewport(0, 0, XRES, YRES);
}

static void errorCallback(int error, const char* description)
{
    cerr << "GLFW error " << error << ": " << description << endl;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    (void) hInstance;
    (void) hPrevInstance;
    (void) lpCmdLine;
    (void) nCmdShow;
#else
int main()
{
#endif // _WIN32
    // Init GLFW-context
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    // Set desired context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    GLFWwindow* windowPtr;
    windowPtr = glfwCreateWindow(XRES, YRES, WINDOW_TITLE, NULL, NULL);
    if (!windowPtr) {
        glfwTerminate();
        cerr << "Error creating GLFW-window!" << endl;
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(windowPtr);

    // Init GL
    if (gl3wInit()) {
        glfwDestroyWindow(windowPtr);
        glfwTerminate();
        cerr << "Error initializing GL3W!" << endl;
        exit(EXIT_FAILURE);
    }

    // Set vsync on
    glfwSwapInterval(1);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {
        glfwDestroyWindow(windowPtr);
        glfwTerminate();
        cerr << "Error initializing GL!" << endl;
        exit(EXIT_FAILURE);
    }

    // Setup imgui
    ImGui_ImplGlfwGL3_Init(windowPtr, true);
    ImGuiWindowFlags logWindowFlags= 0;
    logWindowFlags |= ImGuiWindowFlags_NoTitleBar;
    logWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    bool showLog = true;

    Logger logger;
    logger.AddLog("[gl] Context: %s\n     GLSL: %s\n",
                   glGetString(GL_VERSION),
                   glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Set glfw-callbacks, these will pass to imgui's callbacks if overridden
    glfwSetWindowSizeCallback(windowPtr, windowSizeCallback);
    glfwSetKeyCallback(windowPtr, keyCallback);
    glfwSetCursorPosCallback(windowPtr, cursorCallback);
    glfwSetMouseButtonCallback(windowPtr, mouseButtonCallback);

    // Capture cout for logging
    std::stringstream logCout;
    std::streambuf* oldCout = std::cout.rdbuf(logCout.rdbuf());

    // Load shaders
    std::string vertPath(RES_DIRECTORY);
    vertPath += "shader/basic_vert.glsl";
    std::string rmFragPath(RES_DIRECTORY);
    rmFragPath += "shader/basic_frag.glsl";
    ShaderProgram rmShader(vertPath, rmFragPath);

    std::string fbmFragPath(RES_DIRECTORY);
    fbmFragPath += "shader/fbm_frag.glsl";
    ShaderProgram fbmShader(vertPath, fbmFragPath);

    Quad q;

    // Generate fbm-texture in a framebuffer
    uint32_t noiseW = 1024;
    uint32_t noiseH = noiseW;
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    Texture fbmTex(noiseW, noiseH, TextureParams{GL_R32F, GL_RED, GL_FLOAT,
                                                 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                                                 GL_REPEAT, GL_REPEAT});
    fbmTex.bindWrite(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Generate noise to texture on gpu
    glViewport(0, 0, noiseW, noiseH);
    fbmShader.bind();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec2 res(noiseW, noiseH);
    glUniform2fv(fbmShader.getULoc("uRes"), 1, glm::value_ptr(res));
    q.render();
    fbmTex.genMipmap();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Set actual viewport size
    glViewport(0, 0, XRES, YRES);

    // Set up audio
    std::string musicPath(RES_DIRECTORY);
    musicPath += "music/illegal_af.mp3";
    AudioStream::getInstance().init(musicPath, 174.0, 8);
    int32_t streamHandle = AudioStream::getInstance().getStreamHandle();

    // Set up rocket
    sync_device *rocket = sync_create_device("sync");
    if (!rocket) cout << "[rocket] failed to init" << endl;

    // TODO: playback from file
    int rocketConnected = sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT) == 0;
    if (!rocketConnected)
        cout << "[rocket] failed to connect" << endl;

    // Set up sync tracks
    const sync_track *pulse = sync_get_track(rocket, "pulse");

    Timer rT;
    Timer gT;

    if (rocketConnected) AudioStream::getInstance().play();
    // Run the main loop
    while (!glfwWindowShouldClose(windowPtr)) {
        glfwPollEvents();

        // Sync
        double syncRow = 0.0;
        if (rocketConnected) {
            syncRow = AudioStream::getInstance().getRow();
            if (sync_update(rocket, (int)floor(syncRow), &audioSync, (void *)&streamHandle))
                sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT);
        }

        ImGui_ImplGlfwGL3_NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update imgui
        {
            ImGui::SetNextWindowSize(ImVec2(LOGW, LOGH), ImGuiSetCond_Once);
            ImGui::SetNextWindowPos(ImVec2(LOGM, YRES - LOGH - LOGM), ImGuiSetCond_Always);
            ImGui::Begin("Log", &showLog, logWindowFlags);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            if (logCout.str().length() != 0) {
                logger.AddLog("%s", logCout.str().c_str());
                logCout.str("");
            }
            logger.Draw();
            ImGui::End();
        }

        // Try reloading the shader every 0.5s
        if (rT.getSeconds() > 0.5f) {
            rmShader.reload();
            rT.reset();
        }

        if (rmShader.isLinked()) {
            rmShader.bind();
            glUniform1f(rmShader.getULoc("uGT"), gT.getSeconds());
            glm::vec2 res(XRES,YRES);
            glUniform2fv(rmShader.getULoc("uRes"), 1, glm::value_ptr(res));
            glUniform2fv(rmShader.getULoc("uMPos"), 1, glm::value_ptr(CURSOR_POS));
            if (rocketConnected)
                glUniform1f(rmShader.getULoc("uPulse"), (float)sync_get_val(pulse, syncRow));
            fbmTex.bindRead(GL_TEXTURE0, rmShader.getULoc("uFbmSampler"));
            q.render();
        }

        ImGui::Render();
        glfwSwapBuffers(windowPtr);
    }

    // Save rocket tracks
    sync_save_tracks(rocket);

    // Release resources
    sync_destroy_device(rocket);
    std::cout.rdbuf(oldCout);
    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(windowPtr);
    glfwTerminate();
    glDeleteBuffers(1, &fbo);

    exit(EXIT_SUCCESS);
}
