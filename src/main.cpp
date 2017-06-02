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
#include <track.h>

#include "audioStream.hpp"
#include "frameBuffer.hpp"
#include "logger.hpp"
#include "quad.hpp"
#include "scene.hpp"
#include "shaderProgram.hpp"
#include "texture.hpp"
#include "timer.hpp"

// Comment out to disable autoplay without tcp-Rocket
//#define MUSIC_AUTOPLAY
// Comment out to load sync from files
#define TCPROCKET
// Comment out to remove gui
#define GUI

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
    bool RESIZED = false;
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
#ifdef GUI
    else
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
#endif // GUI
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        CURSOR_POS = glm::vec2(2 * xpos / XRES - 1.0, 2 * (YRES - ypos) / YRES - 1.0);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
#ifdef GUI
    if (ImGui::IsMouseHoveringAnyWindow()) {
        ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
        return;
    }
#endif //GUI

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
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
    RESIZED = true;
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

#ifdef GUI
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


    // Capture cout for logging
    std::stringstream logCout;
    std::streambuf* oldCout = std::cout.rdbuf(logCout.rdbuf());
#endif // GUI

    // Set glfw-callbacks, these will pass to imgui's callbacks if overridden
    glfwSetWindowSizeCallback(windowPtr, windowSizeCallback);
    glfwSetKeyCallback(windowPtr, keyCallback);
    glfwSetCursorPosCallback(windowPtr, cursorCallback);
    glfwSetMouseButtonCallback(windowPtr, mouseButtonCallback);

    // Load fbm shader
    std::string vertPath(RES_DIRECTORY);
    vertPath += "shader/basic_vert.glsl";
    std::string fbmFragPath(RES_DIRECTORY);
    fbmFragPath += "shader/fbm_frag.glsl";
    ShaderProgram fbmShader(vertPath, fbmFragPath);

    Quad q;

    // Generate fbm-texture in a framebuffer
    uint32_t noiseW = 1024;
    uint32_t noiseH = noiseW;
    std::vector<TextureParams> fbmTexParams({{GL_R32F, GL_RED, GL_FLOAT,
                                              GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                                              GL_REPEAT, GL_REPEAT}});
    FrameBuffer fbmFbo(noiseW, noiseH, fbmTexParams);

    // Generate noise to texture on gpu
    glViewport(0, 0, noiseW, noiseH);
    fbmShader.bind();
    fbmFbo.bindWrite();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec2 res(noiseW, noiseH);
    glUniform2fv(fbmShader.getULoc("uRes"), 1, glm::value_ptr(res));
    q.render();
    fbmFbo.genMipmap(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Set actual viewport size
    glViewport(0, 0, XRES, YRES);

    // Set up audio
    std::string musicPath(RES_DIRECTORY);
    musicPath += "music/illegal_af.mp3";
    AudioStream::getInstance().init(musicPath, 175.0, 8);
    int32_t streamHandle = AudioStream::getInstance().getStreamHandle();

    // Set up rocket
    sync_device *rocket = sync_create_device("sync");
    if (!rocket) cout << "[rocket] failed to init" << endl;

    // Set up scenes
    std::string rmFragPath(RES_DIRECTORY);
    rmFragPath += "shader/basic_frag.glsl";
    Scene scene(std::vector<std::string>({vertPath, rmFragPath}),
                std::vector<std::string>({"uPulse"}), rocket);

    // Generate framebuffer for main rendering
    TextureParams rgb16fParams = {GL_RGB16F, GL_RGB, GL_FLOAT,
                                  GL_LINEAR, GL_LINEAR,
                                  GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER};
    TextureParams rgb16fMipParams = {GL_RGB16F, GL_RGB, GL_FLOAT,
                                     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                                     GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER};
    std::vector<TextureParams> mainTexParams({rgb16fParams, rgb16fParams, rgb16fMipParams});
    FrameBuffer mainFbo(XRES, YRES, mainTexParams);

    // Generate additional buffers
    FrameBuffer pingFbo(XRES, YRES, std::vector<TextureParams>({rgb16fParams}));
    FrameBuffer pongFbo(XRES, YRES, std::vector<TextureParams>({rgb16fParams}));

    // Set up post-processing
    std::string bloomFragPath(RES_DIRECTORY);
    bloomFragPath += "shader/bloom_frag.glsl";
    ShaderProgram bloomShader(vertPath, bloomFragPath);

    std::string tonemapFragPath(RES_DIRECTORY);
    tonemapFragPath += "shader/tonemap_frag.glsl";
    ShaderProgram tonemapShader(vertPath, tonemapFragPath);

    const sync_track* uExposure = sync_get_track(rocket, "uExposure");
    const sync_track* uBloom = sync_get_track(rocket, "uBloom");
    const sync_track* uBloomThreshold = sync_get_track(rocket, "uBloomThreshold");

#ifdef TCPROCKET
    // Try connecting to rocket-server
    int rocketConnected = sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT) == 0;
    if (!rocketConnected)
        cout << "[rocket] failed to connect" << endl;
#endif // TCPROCKET

    Timer rT;
    Timer gT;

#ifdef MUSIC_AUTOPLAY
    AudioStream::getInstance().play();
#endif // MUSIC_AUTOPLAY

    // Run the main loop
    while (!glfwWindowShouldClose(windowPtr)) {
        glfwPollEvents();

        // Resize buffers if windowsize changed
        if (RESIZED) {
            mainFbo.resize(XRES, YRES);
            pingFbo.resize(XRES, YRES);
            pongFbo.resize(XRES, YRES);
            RESIZED = false;
        }

        // Sync
        double syncRow = AudioStream::getInstance().getRow();

#ifdef TCPROCKET
        // Try re-connecting to rocket-server if update fails
        // Drops all the frames, if trying to connect on windows
        if (sync_update(rocket, (int)floor(syncRow), &audioSync, (void *)&streamHandle))
            sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT);
#endif // TCPROCKET

#ifdef GUI
        ImGui_ImplGlfwGL3_NewFrame();
#endif // GUI

#ifdef GUI
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
#endif // GUI

        // Try reloading the shader every 0.5s
        if (rT.getSeconds() > 0.5f) {
            scene.reload();
            bloomShader.reload();
            tonemapShader.reload();
            rT.reset();
        }

        // Set res-vec for use in shaders
        glm::vec2 res(XRES,YRES);

        // Bind scene and main buffers
        glViewport(0, 0, XRES, YRES);
        scene.bind(syncRow);
        mainFbo.bindWrite();
        // Bind global uniforms
        glUniform1f(scene.getULoc("uGT"), gT.getSeconds());
        glUniform2fv(scene.getULoc("uRes"), 1, glm::value_ptr(res));
        glUniform2fv(scene.getULoc("uMPos"), 1, glm::value_ptr(CURSOR_POS));
        glUniform1f(scene.getULoc("uBloomThreshold"), (float)sync_get_val(uBloomThreshold, syncRow));
        fbmFbo.bindRead(0, GL_TEXTURE0, scene.getULoc("uFbmSampler"));
        // Render scene to main buffers
        q.render();
        mainFbo.genMipmap(2);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Calculate bloom
        bloomShader.bind();
        pingFbo.bindWrite();
        glUniform2fv(bloomShader.getULoc("uRes"), 1, glm::value_ptr(res));
        glUniform1f(bloomShader.getULoc("uLOD"), 0.f);
        glUniform1i(bloomShader.getULoc("uHorizontal"), 1);
        mainFbo.bindRead(2, GL_TEXTURE0, bloomShader.getULoc("uBloomSampler"));
        q.render();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        pongFbo.bindWrite();
        pingFbo.bindRead(0, GL_TEXTURE0, bloomShader.getULoc("uBloomSampler"));
        glUniform1i(bloomShader.getULoc("uHorizontal"), 0);
        q.render();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Bind tonemap/gamma -shader and render final frame
        glViewport(0, 0, XRES, YRES);
        tonemapShader.bind();
        glUniform2fv(tonemapShader.getULoc("uRes"), 1, glm::value_ptr(res));
        glUniform1f(tonemapShader.getULoc("uExposure"), (float)sync_get_val(uExposure, syncRow));
        glUniform1f(tonemapShader.getULoc("uBloom"), (float)sync_get_val(uBloom, syncRow));
        mainFbo.bindRead(1, GL_TEXTURE0, tonemapShader.getULoc("uHdrSampler"));
        pongFbo.bindRead(0, GL_TEXTURE1, tonemapShader.getULoc("uBloomSampler"));
        q.render();


#ifdef GUI
        ImGui::Render();
#endif // GUI

        glfwSwapBuffers(windowPtr);
    }

    // Save rocket tracks
    sync_save_tracks(rocket);

    // Release resources
    sync_destroy_device(rocket);

#ifdef GUI
    std::cout.rdbuf(oldCout);
    ImGui_ImplGlfwGL3_Shutdown();
#endif // GUI

    glfwDestroyWindow(windowPtr);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
