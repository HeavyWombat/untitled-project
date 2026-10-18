#ifndef __IMAGE_LOADER_H
#define __IMAGE_LOADER_H

#include <fakelib.h>

/* PNG image loader for macOS */
fake_image_t* load_png_image(const char* filename);
void free_image(fake_image_t *image);

#endif /* __IMAGE_LOADER_H */
