#include "pngLoader.h"
#include <GL/glew.h>
#include <iostream>
#include <SDL_image.h>
#include "../Graphics/Texture.h"
#include "../defines.cpp"

#pragma warning(disable : 4996)

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

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
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3));
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
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		*width = TextureImage->w;
		*height = TextureImage->h;
	}

	/* Free up any memory we may have used */
	if (TextureImage)
		SDL_FreeSurface(TextureImage);

	return texture;
}