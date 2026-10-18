# 3D Shooting Range Demo

A 3D first-person shooting range simulator built with OpenGL and C.

This project was created as part of a university course on 3D graphics programming and served as a showcase project demonstrating real-time 3D rendering, game mechanics, and audio integration.

## Project Overview

- **Type:** 3D Shooting Range Simulator (Demo Project)
- **Purpose:** University 3D graphics course showcase
- **Language:** C (~6,275 lines + game modules)
- **Graphics:** OpenGL (GL/GLU)
- **Audio:** FMOD Ex 4.03.02
- **Platform:** Windows (MinGW/GCC)

## Features

- Shooting range environment with distance markers (100m-300m)
- Rifle mechanics with multiple fire modes (Safe, Single, Multi)
- 3D terrain rendering with trees and environmental objects
- Dynamic weather effects (rain, thunder)
- Skybox rendering for atmospheric environment
- Heads-Up Display (HUD) with shooting feedback
- Spatial audio system for immersive sound effects

## Prerequisites

⚠️ **Asset files not included:** This repository contains source code only. You must supply your own texture and audio assets. See `docs/ASSETS.md` for a complete list of required files and licensing guidance.

## Building

### Windows
```bash
cd build/win
make
```

The compiled executable will be placed in the project root directory.

## Dependencies

- OpenGL 32 / GLU 32
- FMOD Ex 4.03.02
- libfake.a (custom image/archive library)
- zlib

## Project Structure

```
.
├── src/
│   ├── win/          # Windows-specific OS layer
│   ├── add/          # Game modules (19 modules)
│   ├── tinyxml/      # XML parsing library
│   └── *.c           # Core framework
├── include/
│   ├── framework/    # Framework headers
│   └── add/          # Game module headers
├── build/
│   └── win/          # Windows build system
├── data/             # Game assets (textures, models, audio)
└── docs/             # Documentation

```

## Documentation

- `docs/ASSETS.md` - Required asset files and licensing guidance

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
