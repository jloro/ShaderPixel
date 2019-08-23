# ShaderPixel

ShaderPixel is a 42 project. It's a fragment shader museum using OpenGL and C++ and a custom game engine. Most of the shader use raymarching.

<img src="/ressources/screenshots/note.png" width="200">


### Installing

Just git clone and run `make`.

```
git clone https://github.com/jloro/ShaderPixel.git
```

And run `./ShaderPixel`.


## Commands

=== COMMANDS ===

WASD : move camera

SPACE/L-CTRL : Move camera up and down

L-SHIFT : Move faster

R : Recompile shader

0-5 : Post processing filter

keypad - + : Change offset of filter 3-5

ESC : Quit

### Different shaders

`3D mandelbulb with hard shadows and Ambiant Occlusion`

<img src="/ressources/screenshots/mandelbulb.png" width="700">

`3D mandelbox`

<img src="/ressources/screenshots/mandelbox.png" width="700">

`Menger sponge`

<img src="/ressources/screenshots/menger.png" width="700">

`Volumetric cloud with volumetric light`

<img src="/ressources/screenshots/lightvolumetric.png" width="700">

`Volumetric cloud`

<img src="/ressources/screenshots/prettycloud.png" width="700">

`Marble using volumetric raymarching`

<img src="/ressources/screenshots/marble.png" width="700">

`A window to another world`

<img src="/ressources/screenshots/window.png" width="700">

`Koch Snowflake using renderbuffer`

<img src="/ressources/screenshots/koch.png" width="700">

## Built With

* [SDL2](https://www.libsdl.org/) - Library for handling window.
* [STBI](https://github.com/nothings/stb) - C++ library to load image.
* [ASSIMP](https://github.com/assimp/assimp) - C++ library to load model.
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications. 
* [OpenGL](https://www.opengl.org/) - OpenGL

## Authors

* **Jules Loro** - [jloro](https://github.com/jloro)
* **Fabien Chevrey** - [fchevrey](https://github.com/fchevrey)
