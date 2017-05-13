# vkpp: C++ Bindings for Vulkan
vkpp is a header only C++ bindings for the Vulkan C API aiming to improve
the developers' experience and efficiency without introducing extra
runtime costs.


## <img src="./Images/WindowsLogo.png" alt="" height=33px>
A Visual Studio 2017 solution file for compiling all examples is included with the repository.
Both the vkpp library and the examples adopted C++17 new features.

## <img src="./Images/LinuxLogo.png" alt="" height=33px>
Coming soon.

## <img src="./Images/AndroidLogo.png" alt="" height=33px>
Coming soon.

## Examples
The examples are used to verify and demonstrate the efficiency, effectiveness and correctness of vkpp.

They are also used as baselines to prevent regressions.

(Note: Some examples are broken due to recent API changes.)

### [ClearFrame](Sample/ClearFrame/)
<img src="./Sample/ClearFrame/ClearFrame.PNG" height="108px">

<hr>

### [StagingTriangle](Sample/StagingTriangle/)
<img src="./Sample/StagingTriangle/StagingTriangle.PNG" height="108px">

<hr>

### [IndexedTriangle](Sample/IndexedTriangle/)
<img src="./Sample/IndexedTriangle/IndexedTriangle.PNG" height="108px">

<hr>

### [UniformTriangle](Sample/UniformTriangle/)
<img src="./Sample/UniformTriangle/UniformTriangle.PNG" height="108px">

<hr>

### [TexturedTriangle](Sample/TexturedTriangle/)
<img src="./Sample/TexturedTriangle/TexturedTriangle.PNG" height="108px">

<hr>

### [DepthTriangle](Sample/DepthTriangle/)
<img src="./Sample/DepthTriangle/DepthTriangle.PNG" height="108px">

<hr>


## Credits
Thanks to the authors of these libraries:
 - [OpenGL Mathematics (GLM)](https://github.com/g-truc/glm)
 - [stb single-file public domain libraries for C/C++](https://github.com/nothings/stb)
 - [Examples and demos for the new Vulkan API](https://github.com/SaschaWillems/Vulkan)

## License
vkpp is provided under MIT license that can be found in the ``LICENSE.txt``
file. By using, distributing, or contributing to this project,
you agree to the terms and conditions of this license.
