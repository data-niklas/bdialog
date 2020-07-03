#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>

#include "bdialog.h"
#include "config.h"

#define PI 3.141592653589793238


/*

    Cairo rendering

*/


void cairo_set_source_hex(char* hexcode){
    if (strlen(hexcode) == 7)hexcode++;
    double r,g,b;
    int value;
    value = strtol(hexcode, NULL, 16);
    r = ((value >> 16) & 0xFF) / 255.0;
    g = ((value >> 8) & 0xFF) / 255.0;
    b = ((value) & 0xFF) / 255.0;

    cairo_set_source_rgb(cr, r, g, b);
}

void initCairo(){
    surface = cairo_xlib_surface_create(dpy, window,
                                  DefaultVisual(dpy, screen),
                                  wwidth, wheight);
    cairo_xlib_surface_set_size(surface, wwidth, wheight);                              

    cr = cairo_create(surface);
    cairo_select_font_face(cr, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, FONT_SIZE);
}

void renderAll(){
    
    cairo_set_source_hex(BACKGROUND_COLOR);
    cairo_rectangle(cr, 0, 0, wwidth + 2 * xoffset, wheight + 2 * yoffset);
    cairo_fill(cr);
    cairo_set_source_hex(FOREGROUND_COLOR);
    //cairo_paint(cr);
    double x = HORIZONTAL_PADDING + xoffset + (wwidth - textwidth - HORIZONTAL_PADDING * 2) / 2;
    double y = textheight + VERTICAL_PADDING + yoffset;
    //cairo_set_source_rgb(cr,255,255,255);
    //cairo_set_source_rgb(cr,0,0,0);
    for (int i = 0; i < linescount; i++)
    {
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, lines[i]);
        y+=textheight*LINE_HEIGHT;
    }
    renderButtons(False);
    cairo_stroke(cr);
    
}

void renderButtons(int clear){
    if (buttons == NULL)return;

    double height = 2 * BORDER_THICKNESS + 2 * BUTTON_PADDING + textheight;
    double x = BUTTON_MARGIN;
    double y = wheight - BUTTON_VERTICAL_MARGIN - height;
    if (BUTTON_POSITION == 1)x = (wwidth - buttonwidth) / 2;
    else if (BUTTON_POSITION == 2)x = wwidth - buttonwidth - BUTTON_MARGIN;
    x+=xoffset;
    y+=yoffset;


    Button *current = buttons;
    while (current != NULL){
        double degrees = PI / 180.0;
        double width = current->w + 2*BUTTON_PADDING + 2 * BORDER_THICKNESS;
        double centerx = 0;
        if (BUTTON_MIN_WIDTH > width){
            centerx = (BUTTON_MIN_WIDTH - width) / 2;
            width = BUTTON_MIN_WIDTH;
        }


        cairo_new_sub_path (cr);
        cairo_arc (cr, x + width - BORDER_RADIUS, y + BORDER_RADIUS, BORDER_RADIUS, -90 * degrees, 0 * degrees);
        cairo_arc (cr, x + width - BORDER_RADIUS, y + height - BORDER_RADIUS, BORDER_RADIUS, 0 * degrees, 90 * degrees);
        cairo_arc (cr, x + BORDER_RADIUS, y + height - BORDER_RADIUS, BORDER_RADIUS, 90 * degrees, 180 * degrees);
        cairo_arc (cr, x + BORDER_RADIUS, y + BORDER_RADIUS, BORDER_RADIUS, 180 * degrees, 270 * degrees);
        cairo_close_path (cr);

        switch (current->type){
           case Default:
                cairo_set_source_hex (current->hovering ? BUTTON_HOVER_BACKGROUND_COLOR : BUTTON_BACKGROUND_COLOR);
                break;
            case Yes:
                cairo_set_source_hex (current->hovering ? BUTTON_YES_HOVER_BACKGROUND_COLOR : BUTTON_YES_BACKGROUND_COLOR);
                break;
            case No:
                cairo_set_source_hex (current->hovering ? BUTTON_NO_HOVER_BACKGROUND_COLOR : BUTTON_NO_BACKGROUND_COLOR);
                break;
        }
        cairo_fill_preserve (cr);
        cairo_set_source_hex(BUTTON_BORDER_COLOR);
        cairo_set_line_width (cr, BORDER_THICKNESS);
        cairo_stroke(cr);


        switch (current->type){
           case Default:
                cairo_set_source_hex (BUTTON_FOREGROUND_COLOR);
                break;
            case Yes:
                cairo_set_source_hex (BUTTON_YES_FOREGROUND_COLOR);
                break;
            case No:
                cairo_set_source_hex (BUTTON_NO_FOREGROUND_COLOR);
                break;
        }

        cairo_move_to(cr,   x + BUTTON_PADDING + BORDER_THICKNESS + centerx, 
                            y + BORDER_THICKNESS + BUTTON_PADDING + textheight);
        cairo_show_text(cr, current->text);

        int SHADOWS = 0;
        if (SHADOWS){
            // Shitty Shadows
            unsigned int SHADOW_SIZE = 1;
            unsigned int SHADOW_OFFSET = 2;
            cairo_set_source_rgb(cr,0.3,0.3,0.3);
            cairo_move_to(cr, x + BORDER_THICKNESS + SHADOW_OFFSET, y + height);
            cairo_line_to(cr, x + width - 2 * BORDER_THICKNESS, y + height);
            cairo_move_to(cr, x + width, y + SHADOW_OFFSET + BORDER_THICKNESS);
            cairo_line_to(cr, x + width, y + height - 2 * BORDER_THICKNESS);
            cairo_arc (cr, x + width - BORDER_RADIUS + 1, y + height - BORDER_RADIUS + 1, BORDER_RADIUS - 1, 0, 90 * degrees);
            cairo_set_line_width(cr, SHADOW_SIZE);
            cairo_stroke(cr);
        }

        current->x = x;
        current->y = y;

        x += width + BUTTON_MARGIN;
        current = current->next;
    }
}






/*


    Button events


*/

void buttonCheckHover(int x, int y){
    if (buttons == NULL || y < buttons->y - 40)return;
    int h = textheight + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
    Button *current = buttons;
    while (current != NULL){
        int w = current->w + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
        if (BUTTON_MIN_WIDTH>w)w=BUTTON_MIN_WIDTH;
        current->hovering = (x >= current->x) && (y >= current->y) && (x <= current->x + w) && (y <= current->y + h);
        current = current->next;
    }
    renderButtons(True);
}

void buttonCheckPress(int x, int y){
    if (buttons == NULL || y < buttons->y)return;
    int h = textheight + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
    Button *current = buttons;
    while (current != NULL){
        int w = current->w + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
        if (BUTTON_MIN_WIDTH>w)w=BUTTON_MIN_WIDTH;
        if ((x >= current->x) && (y >= current->y) && (x <= current->x + w) && (y <= current->y + h)){
            printf("%s\n", current->text);
            cleanup();
            exit(1);
        }
        current = current->next;
    }
}







/*

    Arguments / Stdin

*/

void parseButtons(char *buttonstring){


    int len = strlen(buttonstring);
    if (len == 0)return;

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    cairo_t *cairo = cairo_create(surface);
    cairo_text_extents_t *extents = malloc(sizeof(cairo_text_extents_t));
    cairo_select_font_face(cairo, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cairo, FONT_SIZE);

    Button *current;
    int last = 0;
    for (int i = 0; i < len; i++)
    {   
        if (buttonstring[i] == '\n'){
            last = addButton(buttonstring, i, last, &current, cairo, extents);

        }
    }



    addButton(buttonstring, len, last, &current, cairo, extents);

    buttonwidth -= BUTTON_MARGIN;

    free(extents);
    cairo_surface_destroy(surface);
    cairo_destroy(cairo);

}

int addButton(char* string, int i, int last, Button **current, cairo_t *cairo, cairo_text_extents_t *extents){
    Button *temp = malloc(sizeof(Button));
    if (last == 0){
        (*current) = temp;
        buttons = temp;
    }
    else{
        (*current)->next = temp;
    }
    temp->x=0;
    temp->y=0;
    temp->hovering=False;
    temp->type=Default;

    int typebit = 0;
    if (string[last] == '!'){
        typebit = 1;
        temp->type = No;
    }
    else if (string[last] == '#'){
        typebit = 1;
        temp->type = Yes;
    }

    
    temp->text = malloc(sizeof(char) * (i - last + 1 - typebit));
    strncpy(temp->text, (string + last + typebit), i - last - typebit);
    temp->text[i - last - typebit] = '\0';

    cairo_text_extents(cairo, temp->text, extents);
    temp->w=extents->width;
    if (textheight == 0)textheight = extents->height;
    int tempwidth = extents->width + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
    if (BUTTON_MIN_WIDTH > tempwidth)tempwidth = BUTTON_MIN_WIDTH;
    buttonwidth += tempwidth + BUTTON_MARGIN;


    (*current) = temp;
    return i + 1;
}



//Note_ Border With needs to be specified before the buttons!,also BUTTON_MIN_WIDTH and so on
//Buttons should come last
void processArguments(int argc, char *argv[]){
    
    int i = 0;
    while (i < argc){
        //General
        if (!strcmp(argv[i],"-t")){
            signal(SIGALRM, timeout);
            alarm(atoi(argv[++i]));
        }
        else if (!strcmp(argv[i], "-b")){
            parseButtons(argv[++i]);
        }
        else if (!strcmp(argv[i], "-title")){
            TITLE = argv[++i];
        }//Font
        else if (!strcmp(argv[i], "-fn")){
            FONT_NAME = argv[++i];
        }
        else if (!strcmp(argv[i], "-fs")){
            FONT_SIZE = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-type")){
            WINDOW_TYPE = argv[++i];
        }
        else if (!strcmp(argv[i], "-lheight")){
            LINE_HEIGHT = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-wp")){
            WINDOW_POSITION = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bp")){
            BUTTON_POSITION = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-x")){
            X = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-y")){
            Y = atoi(argv[++i]);
        }//Sizes
        else if (!strcmp(argv[i], "-vp")){
            VERTICAL_PADDING = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-hp")){
            HORIZONTAL_PADDING = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bp")){
            BUTTON_PADDING = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bm")){
            BUTTON_MARGIN = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bvm")){
            BUTTON_VERTICAL_MARGIN = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-br")){
            BORDER_RADIUS = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bt")){
            BORDER_THICKNESS = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-bmw")){
            BUTTON_MIN_WIDTH = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-wm")){
            WINDOW_MARGIN = atoi(argv[++i]);
        }//Colors
        else if (!strcmp(argv[i], "-bc")){
            BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-fc")){
            FOREGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bbc")){
            BUTTON_BORDER_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bdfc")){
            BUTTON_FOREGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bdbc")){
            BUTTON_BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bdhc")){
            BUTTON_HOVER_BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-byfc")){
            BUTTON_YES_FOREGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bybc")){
            BUTTON_YES_BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-byhc")){
            BUTTON_YES_HOVER_BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bnfc")){
            BUTTON_NO_FOREGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bnbc")){
            BUTTON_NO_BACKGROUND_COLOR = argv[++i];
        }
        else if (!strcmp(argv[i], "-bnhc")){
            BUTTON_NO_HOVER_BACKGROUND_COLOR = argv[++i];
        }

        i++;
    }
}

void readInput(){
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    cairo_t *cairo = cairo_create(surface);
    cairo_text_extents_t *extents = malloc(sizeof(cairo_text_extents_t));
    cairo_select_font_face(cairo, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cairo, FONT_SIZE);

    char buf[BUFFERSIZE];
    lines = malloc(DEFAULT_LINES * sizeof(char*));

    int index = 0;
    while (fgets(buf, sizeof buf, stdin)){
        if (buf[strlen(buf)-1] == '\n') {
            if (index == DEFAULT_LINES)lines = realloc(lines, index * sizeof(char*));
            lines[index] = malloc(sizeof(char) * (strlen(buf)-1));
            buf[strlen(buf)-1]='\0';
            strcpy(lines[index], buf);
            cairo_text_extents(cairo, lines[index++], extents);
            if (extents->height > textheight)textheight = extents->height;
            if (extents->width > textwidth)textwidth = extents->width;
        } else {
            free(lines);
            printf("Buffer size is not big enough");
            exit(1);
        }
    }
    

    linescount = index;

    free(extents);
    cairo_surface_destroy(surface);
    cairo_destroy(cairo);
}










/*

    Window things

*/

void init(){
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL){
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
}


void setStringProperty(char* property, char* value){
    XChangeProperty(dpy, window, 
        XInternAtom(dpy, property, False),
        XInternAtom(dpy, "UTF8_STRING", False),
        8, PropModeReplace,
        (unsigned char *) value,
        sizeof(value));
}


void makeWindow(){

    //Set the width and height
    wwidth = textwidth + HORIZONTAL_PADDING * 2;
    int totalbuttonwidth = buttonwidth + 2 * BUTTON_MARGIN;
    if (totalbuttonwidth > wwidth)wwidth = totalbuttonwidth;
    wheight = (textheight * LINE_HEIGHT) * linescount + VERTICAL_PADDING * 2;
    if (buttons != NULL)wheight += BUTTON_PADDING * 2 + textheight + BORDER_THICKNESS * 2 + BUTTON_VERTICAL_MARGIN;
    xoffset = 0;
    yoffset = 0;

    window_attributes.background_pixel = XWhitePixel(dpy, screen);
    window_attributes.border_pixel = XBlackPixel(dpy, screen);
    window_attributes.override_redirect = False;

    //Create the window
    window = XCreateWindow(dpy,
                root,
                0, 0,
                wwidth, wheight,
                0,
                CopyFromParent,
                InputOutput,
                CopyFromParent,
                CWBackPixel|CWBorderPixel,
                &window_attributes
            );

    // Close on x button
    XSelectInput(dpy, window, ExposureMask|KeyPressMask|ButtonPressMask|PointerMotionMask);
    XSetWMProtocols(dpy, window, &WM_DELETE_WINDOW, 1);

    // Set the title
    XStoreName(dpy, window, TITLE);
    XTextProperty *iconproperty = malloc(sizeof(XTextProperty));
    XStringListToTextProperty(&ICON_NAME,
                               1,
                               iconproperty);
    XSetWMIconName(dpy, window, iconproperty);
    XClassHint ch = {"bdialog", "bdialog"};
    XSetClassHint(dpy, window, &ch);


    Atom type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    char window_type[20 + strlen(WINDOW_TYPE)];
    strcpy(window_type, "_NET_WM_WINDOW_TYPE_");
    strcat(window_type, WINDOW_TYPE);
    long value = XInternAtom(dpy, window_type, False);
    XChangeProperty(dpy, window, type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);

    //Disable resizing for some wms

    free(iconproperty);

    // Show the window
    XMapRaised(dpy, window);

    //Reposition
    if (WINDOW_POSITION >= 0 && WINDOW_POSITION <= 8){
        int x,y;
        unsigned int w,h,b,d;
        Window *r = malloc(sizeof(Window));
        XGetGeometry(dpy, root, r,&x,&y,&w,&h,&b,&d);
        free(r);

        int column = WINDOW_POSITION % 3;
        int row = (WINDOW_POSITION - column) / 3;
        switch (column){
            case 0:
                X = WINDOW_MARGIN;
                break;
            case 1:
                X = (w - wwidth) / 2;
                break;
            case 2:
                X =  w - wheight - WINDOW_MARGIN;
                break;
        }
        switch (row){
            case 0:
                Y = WINDOW_MARGIN;
                break;
            case 1:
                Y = (h - wheight) / 2;
                break;
            case 2:
                Y = h - wheight - WINDOW_MARGIN;
                break;
        }
    }
    XWindowChanges *changes = malloc(sizeof(XWindowChanges));
    unsigned long mask = 0;
    if (X >= 0){
        mask |= CWX;
        changes->x = X; 
    }
    if (Y >= 0){
        mask |= CWY;
        changes->y = Y;
    }

    XConfigureWindow(dpy, window, mask, changes);
    free(changes);

    XFlush(dpy);
}


void cleanup(){
    cairo_surface_destroy(surface);
    cairo_destroy(cr);
    XDestroyWindow(dpy, window);
    XCloseDisplay(dpy);
}

void timeout(int signal){
    //cleanup();
    exit(1);
}


int main(int argc, char *argv[]){
    processArguments(argc, argv);
    readInput();
    init();
    makeWindow();

    while (1)  {
        XNextEvent(dpy, &events);
        switch  (events.type) {
            case Expose:
                if (events.xexpose.count == 0){
                    if (cr == NULL)initCairo();
                    xoffset = (events.xexpose.width - wwidth) / 2;
                    yoffset = (events.xexpose.height - wheight) / 2;
                    cairo_xlib_surface_set_size(surface, events.xexpose.width, events.xexpose.height);
                    renderAll();
                }
            break;
            case KeyPress:
                //Escape key
                if (events.xkey.keycode == 9){
                    goto end;
                }
            break;
            case ButtonPress:
                buttonCheckPress(events.xbutton.x, events.xbutton.y);
            break;
            case MotionNotify:
                buttonCheckHover(events.xmotion.x, events.xmotion.y);
            break;
            case ClientMessage:
                if (events.xclient.data.l[0] == WM_DELETE_WINDOW)goto end;
            break;

                       
        }
    }
    end:
    cleanup();
    return 0;
}