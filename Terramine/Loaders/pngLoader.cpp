#include "pngLoader.h"
#include <GL/glew.h>
//#include <png.h>
//#include <zlib.h>
#include <iostream>
#include <SDL_image.h>
//#include <SOIL.h>
#include "../Graphics/Texture.h"
#include "../defines.cpp"

#pragma warning(disable : 4996)

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

//void readpng_version_info() {
//	fprintf(stderr, "Compiled with libpng%s; using libpng%s.\n", PNG_LIBPNG_VER_STRING, png_libpng_ver);
//	fprintf(stderr, "Compiled with zlib%s; using zlib%s.\n", ZLIB_VERSION, zlib_version);
//}
//int _png_load(const char* file, int* width, int* height) {
//	FILE* f;
//	int is_png, bit_depth, color_type, row_bytes;
//	png_info* info_ptr, * end_info;
//	unsigned int t_width, t_height;
//	unsigned char header[8], * image_data;
//	unsigned char** row_pointers;
//	png_struct* png_ptr;
//	unsigned int texture;
//	int alpha;
//
//	if (!(f = fopen(file, "rb"))) {
//		std::cerr << "libPNG err: Can not open file... " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//	fread(header, 1, 8, f);
//
//	is_png = !png_sig_cmp(header, 0, 8);
//	if (!is_png) {
//		fclose(f);
//		std::cerr << "libPNG err: This is not PNG format " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
//	if (!png_ptr) {
//		fclose(f);
//		std::cerr << "libPNG err: Can not create read struct " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	info_ptr = png_create_info_struct(png_ptr);
//	if (!info_ptr) {
//		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
//		fclose(f);
//		std::cerr << "libPNG err: Can not create info struct " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	end_info = png_create_info_struct(png_ptr);
//	if (!end_info) {
//		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
//		fclose(f);
//		std::cerr << "libPNG err: Can not create end info struct " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(f);
//		std::cerr << "libPNG err..." << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	png_init_io(png_ptr, f);
//	png_set_sig_bytes(png_ptr, 8);
//	png_read_info(png_ptr, info_ptr);
//	png_get_IHDR(png_ptr, info_ptr, &t_width, &t_height, &bit_depth, &color_type, nullptr, nullptr, nullptr);
//	*width = t_width;
//	*height = t_height;
//	png_read_update_info(png_ptr, info_ptr);
//	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
//
//	image_data = (unsigned char*)malloc(row_bytes * t_height * sizeof(unsigned char));
//	if (!image_data) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(f);
//		std::cerr << "libPNG err: image_data " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	row_pointers = (unsigned char**)malloc(t_height * sizeof(unsigned char*));
//	if (!row_pointers) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(f);
//		std::cerr << "libPNG err: row_pointers " << file << " : " << __LINE__ << std::endl;
//		return 0;
//	}
//
//	for (unsigned int i = 0; i < t_height; ++i)
//		row_pointers[t_height - 1 - i] = image_data + i * row_bytes;
//	png_read_image(png_ptr, row_pointers);
//
//	switch (png_get_color_type(png_ptr, info_ptr)) {
//		case PNG_COLOR_TYPE_RGBA:
//			alpha = GL_RGBA;
//			break;
//		case PNG_COLOR_TYPE_RGB:
//			alpha = GL_RGB;
//			break;
//		default:
//			std::cout << "Color type " << png_get_color_type(png_ptr, info_ptr) << " not supported!\n" << std::endl;
//			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//			fclose(f);
//			return 0;
//	}
//
//	glcall(glGenTextures(1, &texture));
//	glcall(glBindTexture(GL_TEXTURE_2D, texture));
//	glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
//	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, alpha, GL_UNSIGNED_BYTE, (void*)image_data));
//	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
//	glcall(glBindTexture(GL_TEXTURE_2D, 0));
//
//	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//	free(image_data);
//	free(row_pointers);
//	fclose(f);
//
//	return texture;
//}
//GLuint png_texture_load(const char* file_name, int* width, int* height) {
//	png_byte header[8];
//
//	FILE* fp = fopen(file_name, "rb");
//	if (fp == 0) {
//		perror(file_name);
//		return 0;
//	}
//
//	// read the header
//	fread(header, 1, 8, fp);
//
//	if (png_sig_cmp(header, 0, 8)) {
//		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
//		fclose(fp);
//		return 0;
//	}
//
//	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//	if (!png_ptr) {
//		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
//		fclose(fp);
//		return 0;
//	}
//
//	// create png info struct
//	png_infop info_ptr = png_create_info_struct(png_ptr);
//	if (!info_ptr) {
//		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
//		fclose(fp);
//		return 0;
//	}
//
//	// create png info struct
//	png_infop end_info = png_create_info_struct(png_ptr);
//	if (!end_info) {
//		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
//		fclose(fp);
//		return 0;
//	}
//
//	// the code in this if statement gets called if libpng encounters an error
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		fprintf(stderr, "error from libpng\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(fp);
//		return 0;
//	}
//
//	// init png reading
//	png_init_io(png_ptr, fp);
//
//	// let libpng know you already read the first 8 bytes
//	png_set_sig_bytes(png_ptr, 8);
//
//	// read all the info up to the image data
//	png_read_info(png_ptr, info_ptr);
//
//	// variables to pass to get info
//	int bit_depth, color_type;
//	png_uint_32 temp_width, temp_height;
//
//	// get info about png
//	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
//				 NULL, NULL, NULL);
//
//	if (width) { *width = temp_width; }
//	if (height) { *height = temp_height; }
//
//	// Update the png info struct.
//	png_read_update_info(png_ptr, info_ptr);
//
//	// Row size in bytes.
//	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
//
//	// glTexImage2d requires rows to be 4-byte aligned
//	rowbytes += 3 - ((rowbytes - 1) % 4);
//
//	// Allocate the image_data as a big block, to be given to opengl
//	png_byte* image_data;
//	image_data = (png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte) + 15);
//	if (image_data == NULL) {
//		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(fp);
//		return 0;
//	}
//
//	// row_pointers is for pointing to image_data for reading the png with libpng
//	png_bytep* row_pointers = (png_bytep*)malloc(temp_height * sizeof(png_bytep));
//	if (row_pointers == NULL) {
//		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		free(image_data);
//		fclose(fp);
//		return 0;
//	}
//
//	// set the individual row_pointers to point at the correct offsets of image_data
//	for (unsigned int i = 0; i < temp_height; i++) {
//		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
//	}
//
//	// read the png into image_data through row_pointers
//	png_read_image(png_ptr, row_pointers);
//
//	// Generate the OpenGL texture object
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	// clean up
//	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//	free(image_data);
//	free(row_pointers);
//	fclose(fp);
//	return texture;
//}
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
Texture* load_texture2(const char* file) {
	int width, height;
	GLuint texture = LoadGLTextures2(file, &width, &height);
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

	/* Create storage space for the texture */
	SDL_Surface* TextureImage;

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if (TextureImage = IMG_Load(filename)) {
		/* Create The Texture */
		glcall(glGenTextures(1, &texture));

		/* Typical Texture Generation Using Data From The Bitmap */

		glcall(glBindTexture(GL_TEXTURE_2D, texture));
		glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		/* Generate The Texture */
		glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w, TextureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->pixels));

		/* Nearest Filtering */
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4));
		glcall(glGenerateMipmap(GL_TEXTURE_2D));

		glcall(glBindTexture(GL_TEXTURE_2D, 0));

		*width = TextureImage->w;
		*height = TextureImage->h;
	}

	/* Free up any memory we may have used */
	if (TextureImage)
		SDL_FreeSurface(TextureImage);

	return texture;
}
GLuint LoadGLTextures2(const char* filename, int* width, int* height) {
	/* Status indicator */
	GLuint texture = 0;

	/* Create storage space for the texture */
	SDL_Surface* TextureImage;

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if (TextureImage = IMG_Load(filename)) {
		/* Create The Texture */
		glcall(glGenTextures(1, &texture));

		/* Typical Texture Generation Using Data From The Bitmap */

		glcall(glBindTexture(GL_TEXTURE_2D, texture));
		glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		/* Generate The Texture */
		glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureImage->w, TextureImage->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->pixels));

		/* Nearest Filtering */
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4));
		glcall(glGenerateMipmap(GL_TEXTURE_2D));

		//glcall(glBindTexture(GL_TEXTURE_2D, 0));

		*width = TextureImage->w;
		*height = TextureImage->h;
	}

	/* Free up any memory we may have used */
	if (TextureImage)
		SDL_FreeSurface(TextureImage);

	return texture;
}
//int LoadSOILTextures(const char* file, int* width, int* height) {
//	unsigned char* image = SOIL_load_image(file, width, height, 0, SOIL_LOAD_RGBA);
//
//	GLuint texture;
//	glcall(glGenTextures(1, &texture));
//
//	/* Typical Texture Generation Using Data From The Bitmap */
//	glcall(glBindTexture(GL_TEXTURE_2D, texture));
//	glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
//
//	/* Generate The Texture */
//	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
//
//	/* Nearest Filtering */
//	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
//	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
//	//glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4));
//	//glcall(glGenerateMipmap(GL_TEXTURE_2D));
//
//	glcall(glBindTexture(GL_TEXTURE_2D, 0));
//
//	SOIL_free_image_data(image);
//	return texture;
//}