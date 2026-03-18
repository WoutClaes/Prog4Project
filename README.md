# Qbert - Programming 4 Assignment

A recreation of the classic 80's arcade game **Q*bert**, built on top of a custom game engine developed in C++ using [SDL3](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm).

This project is part of the Programming 4 course at [DAE (Howest)](https://www.digitalartsandentertainment.be/). The engine is built from the ground up, applying game programming patterns from '[Game Programming Patterns](https://gameprogrammingpatterns.com/)' by [Robert Nystrom](https://github.com/munificent).

[![Windows Build](https://github.com/WoutClaes/Prog4Project/actions/workflows/cmake.yml/badge.svg)](https://github.com/WoutClaes/Prog4Project/actions/workflows/cmake.yml)
[![Emscripten Build](https://github.com/WoutClaes/Prog4Project/actions/workflows/emscripten.yml/badge.svg)](https://github.com/WoutClaes/Prog4Project/actions/workflows/emscripten.yml)
[![Play in Browser](https://img.shields.io/badge/Play-Web%20Build-blue?logo=github)](https://WoutClaes.github.io/Prog4Project/)

## Play in the browser

The latest Emscripten build is automatically published as a GitHub Page after every successful build:

**[WoutClaes.github.io/Prog4Project](https://WoutClaes.github.io/Prog4Project/)**

## Branch structure

| Branch | Purpose |
|---|---|
| `main` | Stable submissions and assignment deliverables |
| `engine-clean` | Clean engine base, independent of the game |
| `qbert` | Active game development |

The idea is to keep the engine and game work separated so the engine can be reused or cleaned up without affecting game-specific code. The `main` branch reflects the state of the project at each submission point.

> Branch structure and workflow may evolve over time as the project develops.

## Building the project

### Windows

Open the root folder in Visual Studio 2022 or Visual Studio Code (with CMake and CMake Tools extensions). The project will be recognized as a CMake project automatically.

### Emscripten (web) — Windows

Install dependencies via [Chocolatey](https://chocolatey.org/):

```
choco install -y cmake
choco install -y emscripten
choco install -y ninja
choco install -y python
```

Then build:

```
mkdir build_web
cd build_web
emcmake cmake ..
emmake ninja
```

Start a local server to preview:

```
python -m http.server
```

Browse to [http://localhost:8000](http://localhost:8000).

### Emscripten (web) — macOS

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
