# skunkwork
A lightweight framework for fooling around with GLSL-shaders, mainly designed for
ray marching. Current features:
  * includes in glsl
  * log window with correctly parsed shader errors even with includes
  * auto-reloading shaders when sources are saved

## Dependencies
Building skunkwork requires OpenGL, [GLFW3](http://www.glfw.org) and
[GLM](http://glm.g-truc.net/0.9.8/index.html) with CMake being able to find
them. [dear imgui](https://github.com/ocornut/imgui) and
[pre-generated gl3w](https://github.com/sndels/libgl3w) are provided as submodules.
MERCURY's [HG_SDF](http://mercury.sexy/hg_sdf) is also included.

## Building
The CMake-build should work^tm on Sierra (make + AppleClang, Xcode) and Windows 10
(Visual Studio 2017). Submodules need be pulled before running cmake:
```
git submodule init
git submodule update
```
