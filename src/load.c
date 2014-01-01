/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

#define LOADING_WIDTH 256
#define LOADING_HEIGHT 64

/****************************************************************************
 ****************************************************************************/
void ShowLoading() {
	int x, y;

	InsertTexture("textures/loading.png");

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	x = WINDOW_WIDTH / 2 - LOADING_WIDTH / 2;
	y = WINDOW_HEIGHT / 2 - LOADING_HEIGHT / 2;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(x, y);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(x, y + LOADING_HEIGHT);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(x + LOADING_WIDTH, y + LOADING_HEIGHT);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(x + LOADING_WIDTH, y);
	glEnd();

	glXSwapBuffers(display, mainWindow);

}


/****************************************************************************
 ****************************************************************************/
void HideLoading() {
}

