#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

typedef enum{
	Default,
	Yes,
	No
} ButtonType;

typedef struct Button{
	unsigned int x,y,w;
	char *text;
	int hovering;
	ButtonType type;
	struct Button *next;
} Button;

int screen;
Display *dpy;
XSetWindowAttributes window_attributes;
XVisualInfo vinfo;
Window window;
Window root;
XEvent events;
cairo_surface_t *surface;
cairo_t *cr;
Button *buttons;

Atom WM_DELETE_WINDOW;



char** lines;
unsigned int textwidth = 0;
unsigned int textheight = 0;
unsigned int buttonwidth = 0;
unsigned int linescount = 0;
unsigned int wwidth = 0;
unsigned int wheight = 0;




void cairo_set_source_hex(char* hexcode);
void initCairo();
void renderAll();
void renderButtons(int clear);

void buttonCheckHover(int x, int y);
void buttonCheckPress(int x, int y);

void parseButtons(char* buttonstring);
void processArguments(int argc, char *argv[]);
void readInput();

void setStringProperty(char* property, char* value);
void init();
void makeWindow();
void cleanup();
void timeout(int signal);

int main(int argc, char *argv[]);