
#include "xfps.h"

static const int STATUS_WIDTH = 1024;
static const int STATUS_HEIGHT = 64;

static unsigned int healthTexture;
static unsigned int aimTexture;

void InitializeStatus() {

	aimTexture = InsertTexture("textures/aim.png");
	healthTexture = InsertTexture("textures/health.png");

}

void DestroyStatus() {
}

void DrawStatus() {
	int x, y;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

	glBindTexture(GL_TEXTURE_2D, healthTexture);
	x = 0;
	y = 0;
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(x, y);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(x, y + 64);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(x + 64, y + 64);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(x + 64, y + 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, aimTexture);
	x = WINDOW_WIDTH / 2 - 32 / 2;
	y = WINDOW_HEIGHT / 2 - 32 / 2;
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(x, y);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(x, y + 32);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(x + 32, y + 32);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(x + 32, y + 0);
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	DrawNumber(GetPlayerHealth(), 68, 16);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

}

