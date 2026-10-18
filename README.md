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
