# skunkwork
This is what happens when a simple shadertoy feature creeps into demotool-territory...

Current features:
  * Includes in glsl
  * Log window with correctly parsed shader errors even with includes
  * Auto-reloading shaders when sources are saved
  * Gpu-"profiler"
    * Holds a vector of last n deltas for averaging
    * Timing instances can't be interleaved because GL_TIMESTAMP doesn't work on OSX
  * Music playback via singleton using BASS
  * Rocket-interface (Windows-build chugs if TCPROCKET is defined but rocket isn't open)
  * Wrapper-classes for most of the common resources (Scene, FrameBuffer, Texture etc.)

I have used [emoon's version](https://github.com/emoon/rocket) as my Rocket-server.

## Shader

Implements PBR-shading along the lines of UE4 and also has simplex noise + fbm.
MERCURY's [HG_SDF](http://mercury.sexy/hg_sdf) is included for CSG. Decent bloom
and hdr/gamma -correction are present as post-stages. One bounce of reflections is
also calculated to a buffer but not used in current rendering. Current scene requires
some rocket-tracks to be set.

## Dependencies
Building skunkwork only requires OpenGL and BASS externally. BASS can be downloaded
[here](https://www.un4seen.com/bass.html). Just drop the header and dylib/so/lib+dll
into `ext/bass/` and `ext/bass/lib/` respectively. [GLFW3](http://www.glfw.org),
[GLM](http://glm.g-truc.net/0.9.8/index.html), [dear imgui](https://github.com/ocornut/imgui),
[librocket](https://github.com/rocket/rocket) and [pre-generated gl3w](https://github.com/sndels/libgl3w)
are provided as submodules.

## Building
The CMake-build should work™ on Sierra (make + AppleClang), Linux (clang) and
Windows 10 (Visual Studio 2017). Submodules need be pulled before running cmake:
`git submodule update --recursive --init`
