# macOS Port - Known Limitations

## OpenGL Compatibility

### Accumulation Buffer Not Supported

The macOS port **disables the accumulation buffer** due to Metal backend limitations. This affects:

**Motion Blur Effect**: The optical scope motion blur feature (which used `glAccum`) is disabled on macOS. The game will render normally without the blur effect when using the optical scope.

**Why**: Modern macOS OpenGL implementation through Metal does not reliably support accumulation buffers. Attempting to create one causes Metal validation failures and crashes.

**Code Location**: `src/gdv_main.c` - The blur rendering code is wrapped in `#if 0` to disable it.

### Workarounds for Future Enhancement

If motion blur is desired, it could be re-implemented using:
1. **Framebuffer Objects (FBO)** - Render to texture and blend multiple frames
2. **Shader-based motion blur** - Use fragment shaders for post-processing
3. **Temporal accumulation** - Manual frame averaging using textures

These would require more significant code changes but would work reliably on modern macOS.

## Other Limitations

### Text Rendering
Font rendering functions (`gdv_os_drawOutlinedText`, `gdv_os_drawBitmappedText`) are stubs. The Windows version uses `wglUseFontBitmaps` and `wglUseFontOutlines` which don't exist on macOS.

**Impact**: UI text may not appear
**Workaround**: Could be implemented with CoreText or FreeType

### Deprecated OpenGL
macOS has deprecated OpenGL in favor of Metal. While the code still works, you may see warnings like:
```
OpenGL is deprecated
```

**Impact**: None currently, but future macOS versions may remove OpenGL entirely
**Long-term solution**: Port to Metal or use MoltenVK

### Performance Notes
- Legacy OpenGL through Metal has some overhead
- Performance is good on Apple Silicon but not as optimized as native Metal would be
- VSync is enabled by default

## Testing

The program should now start successfully and display graphics with fallback textures. Test with:

```bash
./project
```

Expected behavior:
- Window opens
- Checkerboard textures visible (if no assets provided)
- Mouse/keyboard input works
- No motion blur effect when scoped
- No visible UI text
