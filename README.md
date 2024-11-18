https://github.com/user-attachments/assets/64b74160-ff98-4e89-825c-6fd36365fe63

# Background
<i>Luminescence</i> is a high-performance and multi-platform <a href="https://en.wikipedia.org/wiki/Lumines"><i>Lumines</i></a> engine with support for custom assets, skins, and difficulty levels.<br>
The original <i>Lumines</i> games were not too portable or moddable, hence the creation of this project.

# Feature Overview
### Ported Features:
- All basic gameplay mechanics
- Skin Edit

### Unported Features (for now)
- Versus CPU Mode's AI
- Fancy title/pause menus

### New Features:
- Max framerate of 120FPS (compared to <i>Lumines Remastered's</i> 60).
- Multi-platform compatibility.
- Custom grid sizes.
- 5 custom skins made by myself.
- 5 difficulties to start from.

### Controls
- W or joystick hat UP to rotate right
- S or joystick hat DOWN to rotate left
- A or joystick hat LEFT to move left
- D or joystick hat RIGHT to move right
- LEFT MOUSE or joystick hat BUTTON 1 to hard drop
- ESCAPE or joystick button 9 to pause

# Running
Within 'bin' there are three executables. I built these three to demonstrate the aforementioned cross-platform compatibility.
| System       | Executable you should run | Notes                            |
|--------------|---------------------------|----------------------------------|
| Windows >=10 | Luminescence-windows.exe  | [1*]                             |
| Linux x64    | Luminescence-linux-amd64  | Built on Debian 12               |
| Linux ARMv7l | Luminescence-linux-armv7l | Built on Raspbian on a RPI3 [2*] |

[1*] You can't resize the window on Windows (ha) because this breaks the renderer/textures. I need to implement recreation of the renderer + all textures whenever the window resizes. (<a href="https://stackoverflow.com/questions/78801018/created-textures-break-in-sdl2-when-resizing-a-window">Relevant Issue</a>)

[2*] There are slowdown/timing issues within the ARMv7l executable (32 bit OS issue?), but it is still playable. This is due to issues I still have to iron out within <a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a>.

# Building
Building is cross-platform and uses CMake.

Due to the similar dependencies, you could also refer to <a href="https://github.com/noahc606/Back-to-Earth">Back to Earth's build steps</a>.

### Include 3rd Party Libraries
If you are on Linux/MacOS, you should have the SDL2 and FFmpeg development libraries installed (Look at the linker flags near the bottom of the CMakeLists.txt). If you are on Windows, the linking step (with the -l) in the CMakeLists.txt can be confusing to get working properly. I recommend using MSYS2 and MinGW for building on Windows. Again, see the CMakeLists.txt for more info on this.

Luminescence also depends on <a href="https://github.com/nlohmann/json">nhlomann-json</a> which is a compact header-only library included within this repo. (within 'include/nhlomann')

### Include NCH-CPP-Utils
Clone the repo <a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a> and go to that repo root's 'include/nch' directory. There, copy or symlink the three directories 'cpp-utils', 'sdl-utils', and 'ffmpeg-utils' into a new 'include/nch' directory within the Luminescence repo's root.

### Building & Running
Once you have the 'src', 'include', and 'bin' folders all in the same place (Luminescence's root), run the following:
```
mkdir build
cmake -Bbuild -H.
cmake --build build
```

For the application to run, it must also have some assets available (within '[root]/bin': 'res' and 'config.json'). It should already be there from when you cloned it.

# Credits
### Game & Assets
<li>Original concept/assets/game series by Tetsuya Mizuguchi & Q Entertainment.</li>
<li>Some free-to-use music/sound effects are credited within 'bin/res/credits.txt'.</li>
<li>Some free-to-use fonts are credited within 'bin/ </li>

### Libraries
<li>The <a href="https://www.libsdl.org/">SDL2</a> libraries (SDL2, SDL2_image, SDL2_mixer, SDL2_ttf).</li>
<li><a href="https://ffmpeg.org/">FFmpeg</a>, which is a video/audio decoding library.</li>
<li><a href="https://github.com/nlohmann/json">nhlomann's JSON for Modern C++</a>, a JSON parsing library. Sublicense is included within 'include/nhlomann'.</li>
<li><a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a>, my own helper library</li>

### A NOTE ON ASSETS:
For obvious reasons, I won't be including within this project most assets which belong to Q Entertainment. If you own any game(s) within the <i>Lumines</i> series you could extract the assets from those. Then it's as simple as dragging, dropping, and renaming them into modules within 'data/skins' (see 'res/skins' for examples on how to do this).
