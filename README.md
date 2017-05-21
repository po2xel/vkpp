# vkpp: C++ Bindings for Vulkan
vkpp is a header only C++ bindings for the Vulkan C API aiming to improve
the developers' experience and efficiency without introducing extra
runtime costs.

## <img src="./Images/WindowsLogo.png" alt="" height=33px>
A Visual Studio 2017 solution file for compiling all examples is included with the repository.<br/>
Both the vkpp library and the examples use C++17 new features.

## <img src="./Images/LinuxLogo.png" alt="" height=33px>
Coming soon.

## <img src="./Images/AndroidLogo.png" alt="" height=33px>
Coming soon.

## Examples
The examples are used to verify and demonstrate the correctness efficiency and effectiveness of vkpp.<br/>
They are also used as baselines to prevent regressions.<br/>

***Note: The examples below are originally created by [SaschaWillems](https://github.com/SaschaWillems/Vulkan). I rewrite some of the examples in order to verify vkpp's correctness, efficiency and effectiveness.***

### [Colorized Triangle](Sample/ColorizedTriangle/)
<img src="./Sample/ColorizedTriangle/ColorizedTriangle.png" height="108px" align="right">

Draw a simple gradient triangle using staging buffers.

Vertex and index data are mapped to host visible memory (staging buffers) before they are transferred to device local memory.

### [Multi-Pipelines](Sample/MultiPipelines/)
<img src="./Sample/MultiPipelines/MultiPipelines.png" height="108px" align="right">

Three different pipeline state objects are used here for rendering the same scene with different visual effects.

### [Textured Plate](Sample/TexturedPlate/)
<img src="./Sample/TexturedPlate/TexturedPlate.png" height="108px" align="right">

Texture data, including all mip-levels, is mapped to host visible memory (staing buffer) before it is transferred to device local image memory.

## Credits
Thanks to the authors of these libraries:
 - [Examples and demos for the new Vulkan API](https://github.com/SaschaWillems/Vulkan)
 - [OpenGL Mathematics (GLM)](https://github.com/g-truc/glm)
 - [stb single-file public domain libraries for C/C++](https://github.com/nothings/stb)
 - [Open Asset Import Library](https://github.com/assimp/assimp)

## License
vkpp is provided under MIT license that can be found in the ``LICENSE.txt``
file. By using, distributing, or contributing to this project,
you agree to the terms and conditions of this license.
