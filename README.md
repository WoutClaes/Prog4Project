# Q*bert Programming 4 Assignment

A recreation of the classic 80's arcade game **Q*bert**, built on top of a custom C++ engine using [SDL3](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm).  
Developed as part of the Programming 4 course at [DAE (Howest)](https://www.digitalartsandentertainment.be/), following architecture and patterns inspired by *Game Programming Patterns* by Robert Nystrom.

[![Windows Build](https://github.com/WoutClaes/Prog4Project/actions/workflows/cmake.yml/badge.svg)](https://github.com/WoutClaes/Prog4Project/actions/workflows/cmake.yml)
[![Emscripten Build](https://github.com/WoutClaes/Prog4Project/actions/workflows/emscripten.yml/badge.svg)](https://github.com/WoutClaes/Prog4Project/actions/workflows/emscripten.yml)
[![Play in Browser](https://img.shields.io/badge/Play-Web%20Build-blue?logo=github)](https://WoutClaes.github.io/Prog4Project/)

---

## Play in the Browser

A web build is automatically deployed through GitHub Pages after each successful Emscripten CI run:

**https://WoutClaes.github.io/Prog4Project/**

> **Note:** The current Emscripten build does not function so the link will just bring you to an empty page.

---

## Branch Structure

| Branch | Purpose |
|--------|---------|
| **`main`** | Final submission branch and stable snapshots |
| **`qbert`** | Primary development branch for the game |
| **`engine-clean`** | Standalone engine base without game-specific code |

Most development happened on the **`qbert`** branch.  
For the final submission, all work was merged into **`main`**, which now represents the complete and stable version of the engine and game.

---

## Engine Architecture

The project is split into two CMake targets:

- **`minigin_lib`** the custom engine  
- **`qbert`** the game built on top of the engine

### Build Configuration

All configurations compile with:

- **Warning level 4**
- **Warnings as errors**

### Core Engine Systems

- Game loop (fixed update + variable rendering)
- Scene and game object management
- Component-based architecture
- Input handling (keyboard + controller)
- Audio system (threaded playback)
- Resource management (textures, fonts, audio)
- Event/Observer system
- State machines (e.g., Coily egg -> snake)
- JSON-driven level loading

### Dependencies (via CMake)

- SDL3, SDL3_ttf  
- GLM  
- ImGui  
- nlohmann_json  

---

## Threading

A dedicated **audio thread** ensures:

- Non-blocking sound playback  
- Smooth main loop  
- Reduced stutter during frequent SFX triggers  

---

## Level Loading (JSON)

Levels are fully data-driven and defined in external JSON files.

Each level specifies:

- Base, intermediate, and target cube colors  
- Number of steps required to reach the target color  
- Whether cubes revert when stepped on  
- Bonus score for completing the stage  
- Disk layout  
- Enemy spawn configuration  

This allows new levels to be added without modifying C++ code.

---

## Controls

### Debug / Utility
- **F1** = Skip level  
- **F2** = Mute/unmute  
- **F3** = Volume down  
- **F4** = Volume up  

### Movement (keyboard / numpad / controller)
- **W / Numpad 9 / DPad Up** = Up-right  
- **A / Numpad 7 / DPad Left** = Up-left  
- **S / Numpad 1 / DPad Down** = Down-left  
- **D / Numpad 3 / DPad Right** = Down-right  

---

## Game Modes

### **Single Player**
Classic Q*bert gameplay with keyboard or controller.

### **Coop**
Two Q*berts starting at bottom opposite corners of the pyramid.

### **Versus**
Player-controlled Coily vs player-controlled Q*bert.

### Input Support
- Keyboard + gamepad  
- Two gamepads  

---

## Building the Project

### Windows (Native)

Open the root folder in **Visual Studio 2026** (with CMake Tools).
The project is detected automatically as a CMake project.

---

### Web Build (Emscripten) Windows

Install dependencies via Chocolatey:


```
choco install -y cmake
choco install -y emscripten
choco install -y ninja
choco install -y python
```

Build:

```
mkdir build_web
cd build_web
emcmake cmake ..
emmake ninja
```

Run locally:

```
python -m http.server
```

Open: http://localhost:8000

---

### Web Build (Emscripten) macOS

```
brew install cmake
brew install emscripten
brew install python
```

```
mkdir build_web
cd build_web
emcmake cmake .. -DCMAKE_OSX_ARCHITECTURES=""
emmake make
```

```
python3 -m http.server
```

---

## ! Known Issues

- **Emscripten build is currently not working**

---