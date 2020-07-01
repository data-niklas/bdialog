#define BUFFERSIZE 256

char* TITLE="BDialog";
char* ICON_NAME = "BDialog";

//Can be DIALOG / DOCK / MENU / TOOLBAR
char* WINDOW_TYPE = "DIALOG";

// Paddings and sizes
unsigned int VERTICAL_PADDING = 16;
unsigned int HORIZONTAL_PADDING = 48;
unsigned int BUTTON_PADDING = 8;
unsigned int BUTTON_MARGIN = 4;
unsigned int BUTTON_VERTICAL_MARGIN = 2;
unsigned int BORDER_RADIUS = 4;
unsigned int BORDER_THICKNESS = 0;
unsigned int BUTTON_MIN_WIDTH = 50;
unsigned int WINDOW_MARGIN = 20;


//X and Y position of the window / -1 means unset (e.g.: It is positioned by your wm)
int X = -1;
int Y = -1;
/* Value between 0 and 8, positions the window from the top left corner to the bottom right corner
0   1   2
3   4   5
6   7   8
It will change the X and Y values / By default it is centered
*/
int WINDOW_POSITION = 4;


// 0 (Left) 1 (Center) 2 (Right)    Sets the alignment of the buttons
unsigned int BUTTON_POSITION = 2;
double LINE_HEIGHT = 1.35;

// This is just used for buffering purposes
unsigned int DEFAULT_LINES = 2;


// All the colors
char* BACKGROUND_COLOR = "#FFFFFF";
char* FOREGROUND_COLOR = "#000000";

char* BUTTON_BACKGROUND_COLOR = "#EFEFEF";
char* BUTTON_HOVER_BACKGROUND_COLOR = "#DDDDDD";
char* BUTTON_FOREGROUND_COLOR = "#000000";


// Render a button as a "Yes" button by prepending a '#' before the text, e.g. "bdialog -b '#Yes'"
char* BUTTON_YES_BACKGROUND_COLOR = "#7eed00";
char* BUTTON_YES_HOVER_BACKGROUND_COLOR = "#88cd04";
char* BUTTON_YES_FOREGROUND_COLOR = "#000000";

//Same as yes, but use a '!'
char* BUTTON_NO_BACKGROUND_COLOR = "#F03D43";
char* BUTTON_NO_HOVER_BACKGROUND_COLOR = "#DC3F45";
char* BUTTON_NO_FOREGROUND_COLOR = "#000000";

char* BUTTON_BORDER_COLOR = "#DDDDDD";

char* FONT_NAME = "Arial";
double FONT_SIZE = 16;
