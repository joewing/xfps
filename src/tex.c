/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

static ImageNode *images = NULL;

/****************************************************************************
 ****************************************************************************/
ImageNode *ReadImage(const char *fileName) {
	png_structp pngData = NULL;
	png_infop pngInfo = NULL;
	png_infop pngEndInfo = NULL;
	int bitDepth, colorType;
	ImageNode *np = NULL;
	unsigned char **rows = NULL;
	FILE *fd = NULL;
	static unsigned char header[8];
	int x, y;

	fd = fopen(fileName, "rb");
	if(!fd) {
		Warning("could not open texture: \"%s\"", fileName);
		return NULL;
	}
	fread(header, 1, sizeof(header), fd);
	if(png_sig_cmp(header, 0, sizeof(header))) {
		fclose(fd);
		Warning("PNG image invalid: \"%s\"", fileName);
		return NULL;
	}

	np = Allocate(sizeof(ImageNode));
	np->name = Allocate(strlen(fileName) + 1);
	strcpy(np->name, fileName);
	np->data = NULL;

	pngData = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pngData) {
		Release(np);
		fclose(fd);
		Warning("could not create PNG read struct: \"%s\"", fileName);
		return NULL;
	}

	if(setjmp(png_jmpbuf(pngData))) {
		png_destroy_read_struct(&pngData, &pngInfo, &pngEndInfo);
		if(fd) {
			fclose(fd);
		}
		if(rows) {
			Release(rows);
		}
		if(np) {
			if(np->name) {
				Release(np->name);
			}
			if(np->data) {
				Release(np->data);
			}
			Release(np);
		}
		return NULL;
	}

	pngInfo = png_create_info_struct(pngData);
	if(!pngInfo) {
		png_destroy_read_struct(&pngData, NULL, NULL);
		Release(np);
		fclose(fd);
		Warning("could not create PNG info struct: \"%s\"", fileName);
		return NULL;
	}

	pngEndInfo = png_create_info_struct(pngData);
	if(!pngEndInfo) {
		png_destroy_read_struct(&pngData, &pngInfo, NULL);
		Release(np);
		fclose(fd);
		Warning("could not create PNG end info struct: \"%s\"", fileName);
		return NULL;
	}

	png_init_io(pngData, fd);
	png_set_sig_bytes(pngData, sizeof(header));

	png_read_info(pngData, pngInfo);

	png_get_IHDR(pngData, pngInfo, &np->width, &np->height,
		&bitDepth, &colorType, NULL, NULL, NULL);

	png_set_expand(pngData);

	if(bitDepth == 16) {
		png_set_strip_16(pngData);
	} else if(bitDepth < 8) {
		png_set_packing(pngData);
	}

	png_set_filler(pngData, 0xFF, PNG_FILLER_AFTER);

	if(colorType == PNG_COLOR_TYPE_GRAY
		|| colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(pngData);
	}

	png_read_update_info(pngData, pngInfo);

	np->data = Allocate(4 * np->width * np->height);

	rows = Allocate(np->height * sizeof(np->data));

	y = 0;
	for(x = 0; x < np->height; x++) {
		rows[x] = &np->data[y];
		y += np->width * 4;
	}

	png_read_image(pngData, rows);

	png_read_end(pngData, pngInfo);
	png_destroy_read_struct(&pngData, &pngInfo, &pngEndInfo);

	fclose(fd);

	Release(rows);

	return np;

}

/****************************************************************************
 ****************************************************************************/
unsigned int InsertTexture(const char *name) {
	ImageNode *np;

	for(np = images; np; np = np->next) {
		if(!strcmp(np->name, name)) {
			return np->id;
		}
	}

	np = ReadImage(name);
	if(!np) {
		return 0;
	}
	np->next = images;
	images = np;

	glGenTextures(1, &np->id);
	glBindTexture(GL_TEXTURE_2D, np->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, np->width, np->height,
		GL_RGBA, GL_UNSIGNED_BYTE, np->data);

	return np->id;

}

/****************************************************************************
 ****************************************************************************/
void InitializeTextures() {
	images = NULL;
}

/****************************************************************************
 ****************************************************************************/
void DestroyTextures() {
	ImageNode *np;

	while(images) {
		np = images->next;
		glDeleteTextures(1, &images->id);
		Release(images->data);
		Release(images->name);
		Release(images);
		images = np;
	}
}

