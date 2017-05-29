# skunkwork
This is what happens when a simple shadertoy feature creeps into demotool-territory...

Current features:
  * Includes in glsl
  * Log window with correctly parsed shader errors even with includes
  * Auto-reloading shaders when sources are saved
  * Texture-class with parameters exposed by constructor
  * Music playback via singleton using BASS
    * Interface for Rocket

I have used [emoon's version](https://github.com/emoon/rocket) as my Rocket-server.

## Basic shader

Implements PBR-materials along the lines of UE4 and also has simplex noise + fbm.
MERCURY's [HG_SDF](http://mercury.sexy/hg_sdf) is included for CSG.

## Dependencies
Building skunkwork requires OpenGL, [GLFW3](http://www.glfw.org) and
[GLM](http://glm.g-truc.net/0.9.8/index.html) with CMake being able to find them.
BASS is also required and it can be downloaded [here](https://www.un4seen.com/bass.html).
Just drop the header and dylib/so/lib into `ext/bass/`.
[dear imgui](https://github.com/ocornut/imgui), [librocket](https://github.com/rocket/rocket)
and [pre-generated gl3w](https://github.com/sndels/libgl3w) are provided as submodules.

## Building
The CMake-build should work^tm on Sierra (make + AppleClang), Linux (clang) and
Windows 10 (Visual Studio 2017). Submodules need be pulled before running cmake:
```
git submodule init
git submodule update
```
