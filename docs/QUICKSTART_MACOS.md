# macOS Quick Start Guide

## Building and Running

### 1. Install Dependencies

```bash
brew install libpng
```

### 2. Build the Project

```bash
cd build/macos
make
```

The executable `project` will be created in the project root directory.

### 3. Run the Program

```bash
cd ../..  # Go to project root
./project
```

## First Run

The program **successfully runs with or without asset files**!

### With Asset Files (as shown in your output):
```
=== Loading Textures ===
Successfully loaded: mark.png (128x128)
Successfully loaded: labels.png (128x128)
...
Successfully loaded: lightning.png (256x512)
=== Texture Loading Complete ===

=== Initializing Audio ===
Successfully loaded: shoot.wav (22050 Hz, 1 channels, 16 bits)
Successfully loaded: reload.wav (22050 Hz, 1 channels, 16 bits)
Warning: No data chunk in WAV file: rain.wav, using silent audio
Warning: No data chunk in WAV file: thunder.wav, using silent audio
=== Audio Initialization Complete ===
```

The application will open a window and run the 3D shooting range simulation.

### Without Asset Files:
```
=== Loading Textures ===
Warning: Failed to open PNG file (tried: mark.png, ../data/mark.png, data/mark.png)
         Using fallback checkerboard texture
...
=== Texture Loading Complete ===

=== Initializing Audio ===
Warning: Failed to open WAV file (tried: shoot.wav, data/shoot.wav, ../data/shoot.wav)
         Using silent audio
...
=== Audio Initialization Complete ===
```

This is normal! The program uses:
- **Checkerboard textures** for missing images
- **Silent audio** for missing sounds
- **Proceeds without crashing**
- **Skips 3D models** if XML files are missing (sign.xml, ground.xml, sack.xml)

## Controls

(Based on original Windows version - may need adjustment)

- **ESC**: Exit
- **Mouse**: Look around
- **WASD**: Movement
- **Space**: Jump
- **Ctrl**: Crouch
- **Mouse Click**: Shoot
- **R**: Reload

## Adding Assets (Optional)

To get the full visual experience:

1. Create a `data/` directory in the project root
2. Add PNG textures and WAV audio files (see `docs/ASSETS_MACOS.md`)
3. Run the program again

Even partial asset sets work - the program will use fallbacks for missing files.

## Troubleshooting

### Program crashes immediately

Check the crash log for Metal/OpenGL errors. Common fixes:
- Ensure you're running on macOS with OpenGL support
- Update to latest macOS version
- Try running from Terminal to see console output

### Window doesn't appear

The program may be initializing. Check console output for errors.

### Black screen

This can happen if:
- OpenGL context failed to initialize (check console)
- All textures failed to load (you should see warnings)
- The game is in an initialization state

### No audio

Audio is optional. If you see "Warning: Failed to open OpenAL device", your system may not support OpenAL, but the program will continue without audio.

## Performance

The macOS port uses:
- Native Cocoa windowing
- Legacy OpenGL (deprecated but still functional)
- OpenAL for audio

Performance should be good on modern Macs, though Apple Silicon may show deprecation warnings for OpenGL.

## Development

### Cleaning Build

```bash
cd build/macos
make clean
```

### Rebuild

```bash
make
```

### Debug Build

Edit `build/macos/Makefile` and change:
```makefile
CFLAGS += -Wall -Wno-deprecated-declarations -O2
```

to:
```makefile
CFLAGS += -Wall -Wno-deprecated-declarations -g -O0
```

Then rebuild.

## Known Issues

1. **OpenGL Deprecation**: macOS deprecated OpenGL in favor of Metal. Warnings are expected.
2. **Retina Display**: May have scaling issues on high-DPI displays
3. **Text Rendering**: Font rendering stubs are not implemented (text won't appear)
4. **Asset Paths**: Uses Windows-style paths in some places (converted at runtime)

## Next Steps

- See `docs/ASSETS_MACOS.md` for asset creation guide
- Check the plan at `.kilo/plans/1792338865884-macos-port-plan.md` for technical details
- Original Windows build is in `build/win/`
