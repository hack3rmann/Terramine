#include "pngLoader.h"
#include <GL/glew.h>
#include <iostream>
#include "../Graphics/Texture.h"
#include "../defines.cpp"
#include <spng.h>
#include <stdio.h>

#pragma warning(disable : 4996)

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

unsigned char* load_texture_bytes(const char* filename, int* width, int* height) {
    FILE* png;
    int result = 0;
    spng_ctx* ctx = NULL;
    unsigned char* image = NULL;

    png = fopen(filename, "rb");

    if (png == nullptr)
        return 0;

    ctx = spng_ctx_new(0);

    if (ctx == nullptr)
        return 0;

    /* Ignore and don't calculate chunk CRC's */
    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

    /* Set memory usage limits for storing standard and unknown chunks,
       this is important when reading untrusted files! */
    size_t limit = 1024 * 1024 * 64;
    spng_set_chunk_limits(ctx, limit, limit);

    /* Set source PNG */
    spng_set_png_file(ctx, png); /* or _buffer(), _stream() */

    struct spng_ihdr ihdr;
    if (spng_get_ihdr(ctx, &ihdr))
        return 0;

    struct spng_plte plte = { 0 };

    result = spng_get_plte(ctx, &plte);
    if (result && result != SPNG_ECHUNKAVAIL)
        return 0;

    size_t image_size, image_width;

    /* Output format, does not depend on source PNG format except for
       SPNG_FMT_PNG, which is the PNG's format in host-endian or
       big-endian for SPNG_FMT_RAW.
       Note that for these two formats <8-bit images are left byte-packed */
    int fmt = SPNG_FMT_PNG;

    /* With SPNG_FMT_PNG indexed color images are output as palette indices,
       pick another format to expand them. */
    if (ihdr.color_type == SPNG_COLOR_TYPE_INDEXED)
        fmt = SPNG_FMT_RGB8;

    if (spng_decoded_image_size(ctx, fmt, &image_size))
        return 0;

    image = (unsigned char*)malloc(image_size);

    if (image == nullptr)
        return 0;

    /* Decode the image in one go */
    /* ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
    if(ret)
    {
        printf("spng_decode_image() error: %s\n", spng_strerror(ret));
        goto error;
    }*/

    /* Alternatively you can decode the image progressively,
       this requires an initialization step. */
	//char* out = new char[image_size];
    if (spng_decode_image(ctx, nullptr, 0, fmt, SPNG_DECODE_PROGRESSIVE))
        return 0;

    /* ihdr.height will always be non-zero if spng_get_ihdr() succeeds */
    image_width = image_size / ihdr.height;

    struct spng_row_info row_info = { 0 };

    do {
        result = spng_get_row_info(ctx, &row_info);
        if (result) break;

        result = spng_decode_row(ctx, image + row_info.row_num * image_width, image_width);
    } while (!result);

	if (result != SPNG_EOI) {
		std::cerr << "PNG error: " << spng_strerror(result) << std::endl;
		return 0;
	}

    if (result == SPNG_ECHUNKAVAIL)
        return 0;

	*width = image_width / 4;
	*height = ihdr.height;

    spng_ctx_free(ctx);

	return image;
}

Texture* load_texture(const char* file) {
	int width, height;
	GLuint texture = LoadGLTextures(file, &width, &height);
	if (texture == 0) {
		std::cout << "Could not load texture " << file << "!\n";

#	    ifdef EXIT_ON_ERROR
		return nullptr;
#	    endif
	}

	return new Texture(texture, width, height);
}
GLuint LoadGLTextures(const char* filename, int* width, int* height) {
	/* Status indicator */
	GLuint texture = 0;

	/* Image bytemap */
	unsigned char* bytes = load_texture_bytes(filename, width, height);

	/* Create The Texture */
	glcall(glGenTextures(1, &texture));

	/* Typical Texture Generation Using Data From The Bitmap */

	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	/* Generate The Texture */
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes));

	/* Nearest Filtering */
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3));
	glcall(glGenerateMipmap(GL_TEXTURE_2D));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3));
	glcall(glGenerateMipmap(GL_TEXTURE_2D));

	glcall(glBindTexture(GL_TEXTURE_2D, 0));

	return texture;
}

Texture* load_textureRGB(const char* file) {
	int width, height;
	GLuint texture = LoadGLTexturesRGB(file, &width, &height);
	if (texture == 0) {
		std::cout << "Could not load texture " << file << "!\n";

#	    ifdef EXIT_ON_ERROR
		return nullptr;
#	    endif
	}

	return new Texture(texture, width, height);
}
GLuint LoadGLTexturesRGB(const char* filename, int* width, int* height) {
	/* Status indicator */
	GLuint texture = 0;

	/* Image bytes */
	unsigned char* bytes = load_texture_bytes(filename, width, height);

	/* Create The Texture */
	glcall(glGenTextures(1, &texture));

	/* Typical Texture Generation Using Data From The Bitmap */

	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	/* Generate The Texture */
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes));

	/* Nearest Filtering */
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4));
	glcall(glGenerateMipmap(GL_TEXTURE_2D));

	//glcall(glBindTexture(GL_TEXTURE_2D, 0));

	return texture;
}

Texture* load_texture_NO_MIPMAP_LINEAR(const char* file) {
	int width, height;
	GLuint texture = LoadGLTextures_NO_MIPMAP_LINEAR(file, &width, &height);
	if (texture == 0) {
		std::cout << "Could not load texture " << file << "!\n";

#	    ifdef EXIT_ON_ERROR
		return nullptr;
#	    endif
	}

	return new Texture(texture, width, height);
}
GLuint LoadGLTextures_NO_MIPMAP_LINEAR(const char* filename, int* width, int* height) {
	/* Status indicator */
	GLuint texture = 0;

	/* Image bytes */
	unsigned char* bytes = load_texture_bytes(filename, width, height);

	/* Create The Texture */
	glcall(glGenTextures(1, &texture));

	/* Typical Texture Generation Using Data From The Bitmap */

	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	/* Generate The Texture */
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes));

	/* Nearest Filtering */
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	return texture;
}