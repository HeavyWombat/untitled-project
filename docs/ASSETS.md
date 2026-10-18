# Asset Requirements

This project requires external asset files that are **not included** in the Git repository. Users must supply their own assets to build and run the game.

## Required Audio Files

The following audio files must be placed in the `data/` directory:

1. **shoot.wav** - Rifle firing sound effect
2. **reload.wav** - Gun reload/cocking sound effect
3. **rain.wav** - Looping rain ambience (should loop seamlessly)
4. **thunder.wav** - Thunder sound effect

**Format:** WAV files, mono or stereo, 22050 Hz recommended

## Required Texture Files

The following PNG texture files must be placed in the `data/` directory:

### Skybox Textures
- **skybox_front.png** - Front face of skybox
- **skybox_back.png** - Back face of skybox
- **skybox_left.png** - Left face of skybox
- **skybox_right.png** - Right face of skybox
- **skybox_top.png** - Top face of skybox
- **skybox_bottom.png** - Bottom face of skybox

### Terrain Textures
- **ground.png** - Primary ground texture
- **heightmap.png** - Terrain height data
- **stoney.png** - Rocky terrain texture
- **veryStoney.png** - Very rocky terrain texture
- **veryStoney768x768.png** - High-res rocky texture

### Weapon Textures
- **g36.png** - G36 rifle texture
- **optScope.png** - Optical scope overlay
- **reflexScope.png** - Reflex scope overlay
- **ba.png** - Weapon component texture
- **mat.png** - Material texture

### Environment Objects
- **tree0.png** - Tree texture variant 1
- **tree1.png** - Tree texture variant 2
- **sack.png** - Sandbag/sack texture
- **sign100.png** - 100m distance marker
- **sign150.png** - 150m distance marker
- **sign200.png** - 200m distance marker
- **sign250.png** - 250m distance marker
- **sign300.png** - 300m distance marker

### Effects & UI
- **rain.png** or **rainDrop.png** - Rain particle texture
- **lightning.png** - Lightning effect texture
- **mark.png** - Marker/crosshair element
- **labels.png** - UI label textures
- **text.png** - Text rendering atlas
- **front.png** - UI front element
- **back.png** - UI background element

**Format:** PNG files with transparency where appropriate

## File Placement

All asset files must be placed in the `data/` directory at the project root:

```
untitled-project/
├── data/
│   ├── shoot.wav
│   ├── reload.wav
│   ├── rain.wav
│   ├── thunder.wav
│   ├── skybox_front.png
│   ├── skybox_back.png
│   └── [... all other assets ...]
├── src/
├── build/
└── README.md
```

The game expects these files at runtime and will fail to load without them.
