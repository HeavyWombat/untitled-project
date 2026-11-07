# 3D Shooting Range Demo

A 3D first-person shooting range simulator built with OpenGL and C.

This project was created as part of a university course on 3D graphics programming and served as a showcase project demonstrating real-time 3D rendering, game mechanics, and audio integration.

## Project Overview

- **Type:** 3D Shooting Range Simulator (Demo Project)
- **Purpose:** University 3D graphics course showcase
- **Language:** C (~6,275 lines + game modules)
- **Graphics:** OpenGL (GL/GLU)
- **Audio:** FMOD Ex 4.03.02 (Windows) / OpenAL (macOS)
- **Platform:** Windows (MinGW/GCC), macOS (Clang)

## Features

- Shooting range environment with distance markers (100m-300m)
- Rifle mechanics with multiple fire modes (Safe, Single, Multi)
- 3D terrain rendering with trees and environmental objects
- Dynamic weather effects (rain, thunder)
- Skybox rendering for atmospheric environment
- Heads-Up Display (HUD) with shooting feedback
- Spatial audio system for immersive sound effects

## Controls

The same list is available in-game via F1.

### General

| Key | Action |
| --- | --- |
| Esc | Quit the program |
| F1 | Toggle the help screen |
| F2 | Toggle motion blur (disabled in the macOS port) |
| F3 | Toggle between free flight and person view |
| F4 | Toggle the light source |
| F5 | Toggle fog |
| F6 | Save a screenshot (writes `Screenshot_###.tga`; unreliable in fullscreen) |
| F7 | Release the mouse for window work / recapture it |
| F8 | Toggle the FPS counter |
| F9 | Toggle test texturing / correct texturing |
| F10 | Cycle polygon mode (filled, wireframe, points) |
| F11 | Toggle coordinate cross and pointer |
| F12 | Toggle fullscreen |

### Movement

Free flight (default camera mode):

| Key | Action |
| --- | --- |
| W / S | Fly forward / backward |
| A / D | Strafe left / right |
| Up / Down arrow | Fly up / down |
| Mouse | Look around |

Person view (after F3):

| Key | Action |
| --- | --- |
| W / S / A / D | Walk |
| F (hold) | Kneel |
| C (hold) | Crouch |
| Mouse | Look around |

### Weapon

| Key | Action |
| --- | --- |
| Left mouse button | Fire (reflex sight or scope view only; hold in burst mode) |
| R | Reload (takes 5 seconds) |
| T | Safe (no firing) |
| G | Single fire |
| B | Burst fire (hold the mouse button to fire continuously) |
| 1 | Person view |
| 2 | Reflex sight (firing enabled) |
| 3 | 3x optical scope (firing enabled) |

Note: the camera mode keys (1, 2, 3) only work after leaving free flight with F3.

### Intro screens

| Key | Action |
| --- | --- |
| Left / Right arrow | Rotate the model |
| Up / Down arrow | Move the model |
| Home / End | Zoom in / out |
| Space | Continue to the 3D scene |

## Prerequisites

⚠️ **Asset files not included:** This repository contains source code only. You must supply your own texture and audio assets.

- **Windows**: See `docs/ASSETS.md` for required files and licensing guidance
- **macOS**: The port includes graceful fallback handling - see `docs/ASSETS_MACOS.md`. The program will run with placeholder textures and silent audio if assets are missing.

## Building

### Windows
```bash
cd build/win
make
```

### macOS
First, install dependencies:
```bash
brew install libpng
```

Then build:
```bash
cd build/macos
make
```

The compiled executable will be placed in the project root directory.

## Dependencies

### Windows
- OpenGL 32 / GLU 32
- FMOD Ex 4.03.02
- libfake.a (custom image/archive library)
- zlib

### macOS
- OpenGL / GLU (system-provided)
- OpenAL (system-provided)
- libpng (install via Homebrew: `brew install libpng`)
- zlib (system-provided)

## Project Structure

```
.
├── src/
│   ├── win/          # Windows-specific OS layer
│   ├── macos/        # macOS-specific OS layer
│   ├── add/          # Game modules (19 modules)
│   ├── tinyxml/      # XML parsing library
│   └── *.c           # Core framework
├── include/
│   ├── framework/    # Framework headers
│   └── add/          # Game module headers
├── build/
│   ├── win/          # Windows build system
│   └── macos/        # macOS build system
├── data/             # Game assets (textures, models, audio)
└── docs/             # Documentation

```

## Documentation

- `docs/ASSETS.md` - Required asset files and licensing guidance (Windows)
- `docs/ASSETS_MACOS.md` - macOS asset loading with fallback handling
- `docs/QUICKSTART_MACOS.md` - Quick start guide for macOS

## Technical Highlights

This demo project showcases:
- Custom platform abstraction layer (OS-independent game logic)
- OpenGL-based 3D rendering pipeline
- Heightmap-based terrain generation
- TinyXML for 3D model data parsing
- Multi-channel audio mixing with FMOD
- Modular game architecture (19 independent modules)

## License

n/a

## Credits

- **TinyXML** - Copyright (c) 2000-2002 Lee Thomason (zlib license)
- **FMOD Ex** - Firelight Technologies Pty, Ltd.

## History

**Original Development:** April 2006
**Context:** University 3D graphics programming course project
