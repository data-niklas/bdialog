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

/*

	The preferred and hopefully initial width and height
	The content will always stay that size

*/
unsigned int wwidth = 0;
unsigned int wheight = 0;

/* 
	The content will always stay centered
	This is achieved by moving rendering the content with the x and y offset
	--> xoffset = (totalwidth - wwidth) / 2

*/
unsigned int xoffset = 0;
unsigned int yoffset = 0;



// Setting the rgb color from a hexstring, e.g.: '#000000'
void cairo_set_source_hex(char* hexcode);
// Initialize the drawing area, first called by the Expose event in the main method
void initCairo();
void renderAll();
void renderButtons(int clear);

void buttonCheckHover(int x, int y);
void buttonCheckPress(int x, int y);

// Parse the buttons from the text after the -b parameter
void parseButtons(char* buttonstring);
int addButton(char* string, int i, int last, Button **current, cairo_t *cairo, cairo_text_extents_t *extents);
void processArguments(int argc, char *argv[]);
// Read the piped input
void readInput();

void setStringProperty(char* property, char* value);
void init();
void makeWindow();
void cleanup();
void timeout(int signal);

int main(int argc, char *argv[]);