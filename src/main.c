/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

int width, height, depth;
int maxSize;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const float MOVE_DELTA = 0.15;
const float JUMP_DELTA = 0.15;
const unsigned long SPEED = 25000;
const int MAX_JUMP_COUNT = 10;
const float GRAVITY = 0.01;
const float TERMINAL_VELOCITY = -0.25;

Display *display;
Window mainWindow;
static GLXContext context;
static int shouldExit = 0;

char *nextLevel = NULL;

static void EventLoop();
static void Redraw();
static void DrawMap();
static void HideCursor();
static void Initialize();
static void Destroy();
static void HandleButtonPress(XButtonEvent *event);
static void HandleMotionEvent(XMotionEvent *event);
static void HandleExit(char press);
static void HandleKey(KeyCode code, char press);
static void HandleKeyPress(XKeyEvent *event);
static void HandleKeyRelease(XKeyEvent *event);

static struct {
   KeySym sym;
   KeyCode code;
   void (*func)(char);
} key_data[] = {
   {XK_Escape,    0,    HandleExit           },
   {XK_S,         0,    MovePlayerLeft       },
   {XK_s,         0,    MovePlayerLeft       },
   {XK_Left,      0,    MovePlayerLeft       },
   {XK_E,         0,    MovePlayerForward    },
   {XK_e,         0,    MovePlayerForward    },
   {XK_Up,        0,    MovePlayerForward    },
   {XK_F,         0,    MovePlayerRight      },
   {XK_f,         0,    MovePlayerRight      },
   {XK_Right,     0,    MovePlayerRight      },
   {XK_D,         0,    MovePlayerBackward   },
   {XK_d,         0,    MovePlayerBackward   },
   {XK_Down,      0,    MovePlayerBackward   },
   {XK_C,         0,    JumpPlayer           },
   {XK_c,         0,    JumpPlayer           },
   {XK_space,     0,    JumpPlayer           }
};

static int glxAttrib[] = {
	GLX_RGBA,
	GLX_RED_SIZE, 1,
	GLX_GREEN_SIZE, 1,
	GLX_BLUE_SIZE, 1,
	GLX_DOUBLEBUFFER,
	GLX_DEPTH_SIZE, 1,
	None
};

static XVisualInfo *vinfo;

#ifdef DEBUG

static void DoStopDebug() {
	StopDebug();
}

#endif

/****************************************************************************
 ****************************************************************************/
int main(int argc, char *argv[]) {
	Colormap cmap;
	XSetWindowAttributes attr;

#ifdef DEBUG
	StartDebug();
	atexit(DoStopDebug);
#endif

	display = XOpenDisplay(NULL);
	if(!display) {
		fprintf(stderr, "error: could not connect to an X server\n");
		return 1;
	}

	vinfo = glXChooseVisual(display, DefaultScreen(display), glxAttrib);
	if(vinfo == NULL) {
		fprintf(stderr, "GLX not supported\n");
		return 1;
	}

	context = glXCreateContext(display, vinfo, None, GL_TRUE);

	cmap = XCreateColormap(display, DefaultRootWindow(display),
		vinfo->visual, AllocNone);

	attr.colormap = cmap;
	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.event_mask = ExposureMask | StructureNotifyMask;

	mainWindow = XCreateWindow(display, DefaultRootWindow(display), 0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0, vinfo->depth, InputOutput,
		vinfo->visual, CWColormap | CWBorderPixel | CWEventMask
		| CWBackPixel, &attr);
	XStoreName(display, mainWindow, "XFPS");

	glXMakeCurrent(display, mainWindow, context);
	XMapWindow(display, mainWindow);

	StartLevel("maps/map.xml");

	EventLoop();

	Destroy();

	glXDestroyContext(display, context);

	XDestroyWindow(display, mainWindow);

	XCloseDisplay(display);

	return 0;
}

/****************************************************************************
 ****************************************************************************/
void HideCursor() {
	XColor dummy;
	char nothing[] = { 0, 0 };
	Cursor cursor;
	Pixmap pixmap;

	dummy.red = 0;
	dummy.green = 0;
	dummy.blue = 0;

	pixmap = XCreateBitmapFromData(display, mainWindow, nothing, 4, 4);
	cursor = XCreatePixmapCursor(display, pixmap, pixmap,
		&dummy, &dummy, 0, 0);
	XDefineCursor(display, mainWindow, cursor);
}

/****************************************************************************
 ****************************************************************************/
void StartLevel(const char *mapName) {

	Destroy();
	Initialize();

	ShowLoading();

	LoadMap(mapName);
	if(!map) {
		fprintf(stderr, "Could not load map: \"%s\"\n", mapName);
		exit(1);
	} else {
		fprintf(stderr, "Map loaded: %d x %d x %d\n", width, height, depth);
	}
	maxSize = width > height ? width : height;

SpawnEnemies(10);

	if(nextLevel) {
		Release(nextLevel);
		nextLevel = NULL;
	}

	HideLoading();

}

/****************************************************************************
 ****************************************************************************/
void Initialize() {

	float shineMatrix[] = { 50.0 };
	float ambientColor[] = { 0.0, 0.0, 0.0, 1.0 };
	float diffuseColor[] = { 1.0, 1.0, 1.0, 1.0 };
   const size_t key_count = sizeof(key_data) / sizeof(key_data[0]);
   size_t i;

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shineMatrix);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for(i = 0; i < key_count; i++) {
      key_data[i].code = XKeysymToKeycode(display, key_data[i].sym);
   }

	InitializeTrig();
	InitializeTextures();
	InitializeBullets();
	InitializeStatus();
	InitializeObjects();
	InitializePlayer();
	InitializeFont();
	InitializeEnemies();

}

/****************************************************************************
 ****************************************************************************/
void Destroy() {
	DestroyBullets();
	DestroyStatus();
	DestroyEnemies();
	DestroyMap();
	DestroyTextures();
}

/****************************************************************************
 ****************************************************************************/
void Redraw() {
	static float lightPosition[] = { 0.0, 0.0, 0.0, 0.0 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	lightPosition[0] = 0.0;
	lightPosition[1] = depth + 1.0;
	lightPosition[2] = 0.0;
	lightPosition[3] = 1.0;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	SetPlayerPerspective();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawMap();
	DrawEnemies();
	DrawBullets();

	DrawStatus();

	glXSwapBuffers(display, mainWindow);

}

/****************************************************************************
 ****************************************************************************/
void DrawMap() {
	ObjectNode *np;

	for(np = map; np; np = np->next) {
		DrawObject(np);
	}

}

/****************************************************************************
 ****************************************************************************/
void EventLoop() {
	static struct timeval last, current;
	static struct timeval frameCurrent, frameLast;
	unsigned long elapsedTime;
	unsigned long frameCount = 0;
	XEvent event;
	int updated;

	XWarpPointer(display, None, mainWindow, 0, 0, 0, 0,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	while(XCheckMaskEvent(display, PointerMotionMask, &event) == True);

	XSelectInput(display, mainWindow, KeyPressMask | KeyReleaseMask
		| PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

	HideCursor();

	Redraw();
	gettimeofday(&last, NULL);

	while(!shouldExit) {

		updated = 0;
		while(XPending(display) > 0) {
			XNextEvent(display, &event);

			switch(event.type) {
			case KeyPress:
				HandleKeyPress(&event.xkey);
				break;
			case KeyRelease:
				HandleKeyRelease(&event.xkey);
				break;
			case ButtonPress:
				HandleButtonPress(&event.xbutton);
				break;
			case MotionNotify:
				HandleMotionEvent(&event.xmotion);
				updated = 1;
				break;
			default:
				break;
			}
		}

		gettimeofday(&current, NULL);
		elapsedTime = (current.tv_sec - last.tv_sec) * 1000000;
		elapsedTime += current.tv_usec - last.tv_usec;

		if(elapsedTime > SPEED) {
			last = current;

			updated |= UpdatePlayer();
			updated |= UpdateBullets();
			updated |= UpdateObjects();
			updated |= UpdateEnemies();

		}

		if(updated) {
			Redraw();
			++frameCount;
		}

		gettimeofday(&frameCurrent, NULL);
		elapsedTime = (frameCurrent.tv_sec - frameLast.tv_sec) * 1000000;
		elapsedTime += frameCurrent.tv_usec - frameLast.tv_usec;

		if(elapsedTime >= 1000000) {
			printf("FPS: %lu\n", frameCount);
			frameCount = 0;
			frameLast = frameCurrent;
		}

		if(nextLevel) {
			StartLevel(nextLevel);
		}

	}

}

/****************************************************************************
 ****************************************************************************/
void HandleButtonPress(XButtonEvent *event) {
	SetPlayerButtonPress(event->button);
}

/****************************************************************************
 ****************************************************************************/
void HandleMotionEvent(XMotionEvent *event) {
	XEvent ne;
	int x, y;

	ne.xmotion = *event;
	while(XCheckMaskEvent(display, PointerMotionMask, &ne) == True);

	x = ne.xmotion.x - WINDOW_WIDTH / 2;
	y = ne.xmotion.y - WINDOW_HEIGHT / 2;

	XWarpPointer(display, None, mainWindow, 0, 0, 0, 0,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	while(XCheckMaskEvent(display, PointerMotionMask, &ne) == True);

	RotatePlayer(y, x);

}

/****************************************************************************
 ****************************************************************************/
void HandleExit(char press) {
   if(press) {
      shouldExit = 1;
   } else {
      exit(0);
   }
}

/****************************************************************************
 ****************************************************************************/
void HandleKey(KeyCode code, char press) {
   const size_t key_count = sizeof(key_data) / sizeof(key_data[0]);
   size_t i;
   for(i = 0; i < key_count; i++) {
      if(key_data[i].code == code) {
         (key_data[i].func)(press);
         return;
      }
   }
}

/****************************************************************************
 ****************************************************************************/
void HandleKeyPress(XKeyEvent *event) {
   HandleKey(event->keycode, 1);
}

/****************************************************************************
 ****************************************************************************/
void HandleKeyRelease(XKeyEvent *event) {
   HandleKey(event->keycode, 0);
}

