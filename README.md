<img src="./app/res/textures/logo.png" alt="Vlither Logo" width="80" height="80">

# Vlither
## A Desktop Client for Slither.io

Vlither is a client for [Slither.io](https://slither.io) which can run outside the browser. It is written in C and uses Vulkan for rendering all the graphics, thus providing better CPU/GPU utilization and ridiculously fast rendering. A lot of the game-side code (particularly `callback.c`, `oef.c`, and `redraw.c`) is a word-for-word C translation of the original JavaScript client ([protocol version 14](https://slither.io/s/game06112461.js)). I have tested it on Windows (AMD and Intel) and Ubuntu (Intel).

### Running Instructions
No dependencies are required to run the program. If your GPU supports at least Vulkan 1.0 (**no** features or extensions are used), you're good to go.
- Download the latest [release](https://github.com/for-loop9/vlither/releases/tag/1.7).
- Enjoy!

### Build Instructions
Prerequisites
- [Premake](https://premake.github.io) for generating makefiles
- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk)
- [Python 3](https://www.python.org/downloads) (for automated shader compilation)
- GCC (mingw-w64 on Windows)
- GDB (mingw-w64 on Windows) if you plan to debug
- Make (mingw-w64 on Windows)
- X11 dependencies (only for Linux, [see here](https://www.glfw.org/docs/latest/compile.html))

On Windows, the program is developed using [MSYS2](https://www.msys2.org) to get access to GCC, GDB, and Make with mingw-w64 tool chain. It has **_not_** been tested with MSVC or Cygwin. It will most likely not work with MSVC out of the box due to the use of POSIX Threads API.

Make sure the `VULKAN_SDK` environment variable is set properly if on Windows.

```bash
git clone https://github.com/for-loop9/vlither.git
cd vlither
py scripts/compile_shaders.py
premake5 --file=build.lua gmake2
make -C build/makefiles config=debug
build/bin/app
```
You may also work on the project with VS Code by opening the `.code-workspace` file. All the build tasks (`configure` first, then `build`) and launch configurations (`debug` only) have been set. Make sure you have the required extensions installed:
- [C/C++ tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [Makefile tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools)
- [Best theme ever](https://monokai.pro/vscode)
## Features

- Supports all 66 skins
- Custom skin
- Server selection
- Zoom
- Low/high quality
- Leaderboard
- Minimap
- Kill count

## Planned
- Mac
- Android

## Tools Used

The following libraries have been used to develop Vlither:

- [GLFW](https://www.glfw.org) - For windowing and handling input.
- [Ignite](./ignite) - My own lightweight wrapper (sort of an engine?) over [Vulkan](https://vulkan.lunarg.com) for rendering fast 2D graphics.
- [Mongoose](https://mongoose.ws) - For networking.
- [Cimgui](https://github.com/cimgui/cimgui) - A C wrapper for [Dear ImGui](https://github.com/ocornut/imgui), a wonderful immediate mode UI library.
- [STB Image](https://github.com/nothings/stb) - For loading images.
- [VMA (Vulkan Memory Allocator)](https://gpuopen.com/vulkan-memory-allocator) - For memory management in Vulkan.

## Gallery
![Image 0](./gallery/ss3.png)
![Image 1](./gallery/ss0.png)
![Image 3](./gallery/ss2.png)
![Image 2](./gallery/ss1.png)

## License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](./LICENSE) file for details.

### Notice
This project was made entirely by one person - me. It is tested by me and a few people in the official [Slither.io Discord server](https://discord.com/invite/slither). Upon coming across any bugs, I fix them. It is possible that you will come across some while using this program. In the off chance that you do, I would really appreciate if you'd let me know by opening up an issue.
