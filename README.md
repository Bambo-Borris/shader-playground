# Shader Playground - In Development

## What is this?
Simple desktop shader playground with support for GLSL fragment shaders. Allows for uniform/texture setting & editing, and copy+paste from Shadertoy.

## Requirements
- A Compiler
- CMake (>=3.16)

## Build & Run Instructions

```
cmake -B build -DBUILD_SHARED_LIBS="FALSE"
cmake --build build --target run
```

## Credits
[Book of Shaders](https://thebookofshaders.com/)

[Shadertoy](https://shadertoy.com/)

[SFML](https://www.sfml-dev.org/)

[imgui](https://github.com/ocornut/imgui)

[imgui-sfml](https://github.com/eliasdaler/imgui-sfml)

[Inigo Quilez](https://iquilezles.org)

Written by Bambo Borris, with some CMake magic by [Chris Thrasher](https://github.com/ChrisThrasher)