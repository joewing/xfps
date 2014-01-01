
#include "xfps.h"

#define FONT_NAME "-*-courier-bold-r-*-*-50-*-*-*-*-*-*-*"
#define FONT_WIDTH 32

static XFontStruct *font;
static int fontList;
static unsigned int first, last;

void InitializeFont() {
	font = XLoadQueryFont(display, FONT_NAME);
	if(font) {

		first = font->min_char_or_byte2;
		last = font->max_char_or_byte2;

      glColor3f(1.0, 0.0, 0.0);
		fontList = glGenLists(last - first + 1);
		if(fontList == 0) {
			fprintf(stderr, "warning: could not get list for font\n");
			XFreeFont(display, font);
			font = NULL;
		} else {
			glXUseXFont(font->fid, first, last - first - 1, fontList);
		}

	} else {
		fprintf(stderr, "warning: could not load font: %s\n", FONT_NAME);
	}
}

void DestroyFont() {
	if(font) {
		XFreeFont(display, font);
		glDeleteLists(fontList, last - first - 1);
	}
}

void DrawString(const char *str, int x, int y) {

	if(!font) {
		return;
	}

   glRasterPos2i(x, y);
   glListBase(fontList);
   glPushAttrib(GL_LIST_BIT  | GL_ENABLE_BIT);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
   glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
   glPopAttrib();

}

void DrawNumber(int number, int x, int y) {
	static char str[16];

	snprintf(str, sizeof(str), "%d", number);
	DrawString(str, x, y);

}

