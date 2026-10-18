#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <fakelib.h>

/* PNG loader implementation for macOS/libpng */

/* Create a fallback texture when PNG loading fails */
static fake_image_t* create_fallback_image(int width, int height) {
    fake_image_t *image = (fake_image_t*)malloc(sizeof(fake_image_t));
    if (!image) {
        return NULL;
    }

    image->width = (unsigned short)width;
    image->height = (unsigned short)height;
    image->mode = FAKE_IMAGE_MODE_RGBA;
    image->palette = NULL;

    /* Allocate image data */
    size_t size = width * height * 4;  /* RGBA */
    image->data = (unsigned char*)malloc(size);
    if (!image->data) {
        free(image);
        return NULL;
    }

    /* Create a simple checkerboard pattern */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            int checker = ((x / 8) + (y / 8)) % 2;
            unsigned char color = checker ? 200 : 100;
            image->data[idx + 0] = color;  /* R */
            image->data[idx + 1] = color;  /* G */
            image->data[idx + 2] = color;  /* B */
            image->data[idx + 3] = 255;    /* A */
        }
    }

    return image;
}

fake_image_t* load_png_image(const char* filename) {
    FILE *fp;
    png_structp png;
    png_infop info;
    png_bytep *row_pointers = NULL;
    fake_image_t *image = NULL;
    char tried_paths[512];
    int found = 0;

    /* Try to open file */
    fp = fopen(filename, "rb");
    snprintf(tried_paths, sizeof(tried_paths), "%s", filename);

    if (!fp) {
        /* Try ../data prefix */
        char alt_path[512];
        snprintf(alt_path, sizeof(alt_path), "../data/%s", filename);
        fp = fopen(alt_path, "rb");
        snprintf(tried_paths, sizeof(tried_paths), "%s, %s", tried_paths, alt_path);

        if (!fp) {
            /* Try data/ prefix */
            snprintf(alt_path, sizeof(alt_path), "data/%s", filename);
            fp = fopen(alt_path, "rb");
            snprintf(tried_paths, sizeof(tried_paths), "%s, %s", tried_paths, alt_path);

            if (!fp) {
                fprintf(stderr, "Warning: Failed to open PNG file (tried: %s)\n", tried_paths);
                fprintf(stderr, "         Using fallback checkerboard texture\n");
                return create_fallback_image(256, 256);
            }
        }
    }

    /* Create PNG read struct */
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        fprintf(stderr, "Warning: PNG read struct creation failed for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        fprintf(stderr, "Warning: PNG info struct creation failed for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        if (row_pointers) {
            for (png_uint_32 y = 0; y < png_get_image_height(png, info); y++) {
                if (row_pointers[y]) free(row_pointers[y]);
            }
            free(row_pointers);
        }
        fclose(fp);
        fprintf(stderr, "Warning: PNG loading error for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_uint_32 width = png_get_image_width(png, info);
    png_uint_32 height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    /* Convert to RGBA8888 format */
    if (bit_depth == 16) {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    /* Allocate image structure */
    image = (fake_image_t*)malloc(sizeof(fake_image_t));
    if (!image) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        fprintf(stderr, "Warning: Memory allocation failed for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    image->width = (unsigned short)width;
    image->height = (unsigned short)height;
    image->mode = FAKE_IMAGE_MODE_RGBA;
    image->palette = NULL;

    /* Allocate image data */
    size_t rowbytes = png_get_rowbytes(png, info);
    image->data = (unsigned char*)malloc(rowbytes * height);
    if (!image->data) {
        free(image);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        fprintf(stderr, "Warning: Image data allocation failed for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    /* Allocate row pointers */
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (!row_pointers) {
        free(image->data);
        free(image);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        fprintf(stderr, "Warning: Row pointer allocation failed for %s, using fallback\n", filename);
        return create_fallback_image(256, 256);
    }

    for (png_uint_32 y = 0; y < height; y++) {
        row_pointers[y] = image->data + y * rowbytes;
    }

    /* Read the image */
    png_read_image(png, row_pointers);

    /* Cleanup */
    free(row_pointers);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);

    printf("Successfully loaded: %s (%dx%d)\n", filename, width, height);

    return image;
}

void free_image(fake_image_t *image) {
    if (image) {
        if (image->data) {
            free(image->data);
        }
        if (image->palette) {
            free(image->palette);
        }
        free(image);
    }
}
