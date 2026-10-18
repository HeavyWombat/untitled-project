# Asset Management

## Overview

The macOS port includes **graceful fallback handling** for missing assets. If texture or audio files are not found, the program will:

- **Textures**: Use procedurally-generated checkerboard patterns (256x256 RGBA)
- **Audio**: Use silent audio buffers

This allows the program to run without crashes even when assets are missing.

## Asset Loading Behavior

### Textures (PNG files)

The image loader tries multiple paths for each texture:
1. `<filename>` (current directory)
2. `../data/<filename>`
3. `data/<filename>`

If all paths fail, a **checkerboard fallback texture** is generated automatically.

### Audio (WAV files)

The audio loader tries:
1. `<filename>` (current directory)
2. `data/<filename>`
3. `../data/<filename>`

If all paths fail, a **1-second silent audio buffer** is created.

## Required Asset Files

### Textures (PNG format)
Located in `data/` directory:

**UI & HUD:**
- `mark.png` - Crosshair/marker
- `labels.png` - Distance labels
- `text.png` - Text rendering
- `ba.png` - UI element
- `front.png` - Block front
- `back.png` - Block back

**Terrain:**
- `heightmap.png` - Terrain heightmap
- `ground.png` - Ground texture
- `veryStoney.png` - Stone texture

**Skybox (6 faces):**
- `skybox_back.png`
- `skybox_bottom.png`
- `skybox_front.png`
- `skybox_left.png`
- `skybox_right.png`
- `skybox_top.png`

**Objects:**
- `sack.png` - Sack texture
- `mat.png` - Mat texture
- `sign100.png` - 100m distance sign
- `sign150.png` - 150m distance sign
- `sign200.png` - 200m distance sign

**Weapons & Scopes:**
- `optScope.png` - Optical scope overlay
- `reflexScope.png` - Reflex sight overlay
- `g36.png` - Rifle silhouette

**Trees:**
- `tree0.png` - Tree texture variant 1
- `tree1.png` - Tree texture variant 2

**Weather:**
- `rainDrop.png` - Rain particle
- `lightning.png` - Lightning effect

### Audio (WAV format)
Located in `data/` directory:

- `shoot.wav` - Gunshot sound
- `reload.wav` - Reload sound
- `rain.wav` - Rain ambient (looping)
- `thunder.wav` - Thunder sound

### 3D Models (XML format)
Located in `data/` directory:

- `sign.xml` - Distance sign model
- `ground.xml` - Ground object model
- `sack.xml` - Sandbag model

## Creating Placeholder Assets

### Quick Test Setup

To run the program for testing, you can create an empty `data/` directory:

```bash
mkdir -p data
```

The program will run with all fallback textures (checkerboards) and silent audio.

### Minimal Functional Setup

For basic visual testing, create at least:

```bash
mkdir -p data

# Create a simple 256x256 gradient heightmap
# (You can use any image editor or ImageMagick)
convert -size 256x256 gradient:black-white data/heightmap.png

# Create simple colored textures
convert -size 256x256 xc:green data/ground.png
convert -size 512x512 gradient:blue-cyan data/skybox_top.png
# ... (create other skybox faces)
```

## Runtime Messages

When the program runs, it will display:

```
=== Loading Textures ===
Warning: Failed to open PNG file (tried: mark.png, ../data/mark.png, data/mark.png)
         Using fallback checkerboard texture
Successfully loaded: data/heightmap.png (256x256)
...
=== Texture Loading Complete ===

=== Initializing Audio ===
Warning: Failed to open WAV file (tried: shoot.wav, data/shoot.wav, ../data/shoot.wav)
         Using silent audio
...
=== Audio Initialization Complete ===
```

## Asset Specifications

### Textures
- **Format**: PNG (RGB or RGBA)
- **Size**: Any size (will be loaded as-is)
- **Recommended**: Power-of-2 dimensions for better compatibility (256x256, 512x512, 1024x1024)

### Audio
- **Format**: WAV (PCM)
- **Sample Rate**: 44.1kHz recommended
- **Channels**: Mono or Stereo
- **Bit Depth**: 8-bit or 16-bit

### 3D Models
- **Format**: TinyXML custom format (exported from Blender)
- **Required**: Vertex positions, normals, UV coordinates, faces

## Troubleshooting

### "Failed to open PNG file" warnings
- Check that files exist in `data/` directory
- Verify file names match exactly (case-sensitive)
- Ensure PNG files are valid (not corrupted)

### Silent audio
- WAV files must be valid PCM format
- Check file permissions
- Verify WAV header is standard RIFF format

### Checkerboard textures everywhere
- No PNG files were found
- Check working directory (run from project root)
- Verify `data/` directory exists

## Performance Notes

- Fallback textures are lightweight (checkerboard pattern, 256KB each)
- Silent audio buffers are minimal (44.1kHz mono, 0.1 seconds)
- The program will run at full speed even with all fallbacks
