# pbr-playground

The original [skunkwork](https://www.github.com/sndels/skunkwork) repo that turned
into a demo base with a hard coded post-processing pipeline. Implements PBR-shading along the
lines of UE4 and also has simplex noise + fbm. MERCURY's [HG_SDF](http://mercury.sexy/hg_sdf)
is included for CSG. Decent bloom and hdr/gamma -correction are present as post-stages.
Current scene requires some rocket-tracks to be set.

## Dependencies
Building only requires OpenGL and BASS externally. BASS can be downloaded
[here](https://www.un4seen.com/bass.html). Just drop the header and dylib/so/lib+dll
into `ext/bass/` and `ext/bass/lib/` respectively. [GLFW3](http://www.glfw.org),
[GLM](http://glm.g-truc.net/0.9.8/index.html), [dear imgui](https://github.com/ocornut/imgui),
[librocket](https://github.com/rocket/rocket) and [pre-generated gl3w](https://github.com/sndels/libgl3w)
are provided as submodules.

## Building
The CMake-build should work™ on Sierra (make + AppleClang), Linux (clang) and
Windows 10 (Visual Studio 2017). Submodules need be pulled before running cmake:
`git submodule update --recursive --init`
