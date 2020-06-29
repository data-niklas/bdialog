#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
    cairo_paint(cr);
    cairo_set_source_hex(FOREGROUND_COLOR);
    //cairo_paint(cr);
    double y = VERTICAL_PADDING + 16;
    //cairo_set_source_rgb(cr,255,255,255);
    //cairo_set_source_rgb(cr,0,0,0);
    for (int i = 0; i < linescount; i++)
    {
        cairo_move_to(cr, HORIZONTAL_PADDING, y);
        cairo_show_text(cr, lines[i]);
        y+=textheight*LINE_HEIGHT;
    }
    renderButtons(False);
    cairo_stroke(cr);
    cairo_surface_flush(surface);
    
}

void renderButtons(int clear){
    if (buttons == NULL)return;

    double height = 2 * BORDER_THICKNESS + 2 * BUTTON_PADDING + textheight;
    double x = BUTTON_MARGIN;
    double y = wheight - BUTTON_VERTICAL_MARGIN - height;
    if (BUTTON_POSITION == 1)x = (wwidth - buttonwidth) / 2;
    else if (BUTTON_POSITION == 2)x = wwidth - buttonwidth - BUTTON_MARGIN;

    /*
        Clearing is never needed, because the new button will be drawn ontop of the old button (pixel perfect)
    if (clear){
        cairo_set_source_hex(BACKGROUND_COLOR);
        cairo_rectangle(cr, x, y, buttonwidth, height);
        cairo_fill(cr);
    }
    */


    Button *current = buttons;
    while (current != NULL){
        double degrees = PI / 180.0;
        double width = current->w + 2*BUTTON_PADDING + 2 * BORDER_THICKNESS;
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
        cairo_move_to(cr, x + BUTTON_PADDING + BORDER_THICKNESS, y + BORDER_THICKNESS + BUTTON_PADDING + textheight);
        cairo_show_text(cr, current->text);

        current->x = x;
        current->y = y;

        x += width + BUTTON_MARGIN;
        current = current->next;
    }
}












void buttonCheckHover(int x, int y){
    if (buttons == NULL || y < buttons->y)return;
    int h = textheight + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
    Button *current = buttons;
    while (current != NULL){
        int w = current->w + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS;
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
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    cairo_t *cairo = cairo_create(surface);
    cairo_text_extents_t *extents = malloc(sizeof(cairo_text_extents_t));
    cairo_select_font_face(cairo, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cairo, FONT_SIZE);

    Button *current, *temp;
    int len = strlen(buttonstring);
    int last = 0;
    for (int i = 0; i < len; i++)
    {   
        if (buttonstring[i] == '\n'){
            temp = malloc(sizeof(Button));
            if (last == 0){
                current = temp;
                buttons = temp;
            }
            else{
                current->next = temp;
            }

            temp->x=0;
            temp->y=0;
            temp->hovering=False;
            temp->type=Default;

            int typebit = 0;
            if (buttonstring[last] == '!'){
                typebit = 1;
                temp->type = No;
            }
            else if (buttonstring[last] == '#'){
                typebit = 1;
                temp->type = Yes;
            }

            
            temp->text = malloc(sizeof(char) * (i - last + 1 - typebit));
            strncpy(temp->text, (buttonstring + last + typebit), i - last - typebit);
            temp->text[i - last - typebit] = '\0';

            cairo_text_extents(cairo, temp->text, extents);
            temp->w=extents->width;
            buttonwidth += extents->width + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS + BUTTON_MARGIN;


            current = temp;
            last = i + 1;

        }
    }



    temp = malloc(sizeof(Button));
    if (last == 0){
        buttons = temp;
    }
    else{
        current->next = temp;
    }

    temp->x=0;
    temp->y=0;
    temp->hovering=False;            

    int typebit = 0;
    if (buttonstring[last] == '!'){
        typebit = 1;
        temp->type = No;
    }
    else if (buttonstring[last] == '#'){
        typebit = 1;
        temp->type = Yes;
    }

    
    temp->text = malloc(sizeof(char) * (len - last + 1 - typebit));
    strncpy(temp->text, (buttonstring + last + typebit), len - last - typebit);
    temp->text[len - last - typebit] = '\0';

    cairo_text_extents(cairo, temp->text, extents);
    buttonwidth += extents->width + 2 * BUTTON_PADDING + 2 * BORDER_THICKNESS + BUTTON_MARGIN;
    temp->w = extents->width;
    temp->next = NULL;

    buttonwidth -= BUTTON_MARGIN;

    
    free(extents);
    cairo_surface_destroy(surface);
    cairo_destroy(cairo);

}


void processArguments(int argc, char *argv[]){
    
    int i = 0;
    while (i < argc){
        if (!strcmp(argv[i],"-t")){
            signal(SIGALRM, timeout);
            alarm(atoi(argv[++i]));
        }
        else if (!strcmp(argv[i], "-b")){
            parseButtons(argv[++i]);
        }
        else if (!strcmp(argv[i], "-title")){
            TITLE = argv[++i];
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
    wwidth = textwidth + HORIZONTAL_PADDING * 2;
    int totalbuttonwidth = buttonwidth + 2 * BUTTON_MARGIN;
    if (totalbuttonwidth > wwidth)wwidth = totalbuttonwidth;
    wheight = (textheight * LINE_HEIGHT) * linescount + VERTICAL_PADDING * 2;
    if (buttons != NULL)wheight += BUTTON_PADDING * 2 + textheight;
    window_attributes.background_pixel = XWhitePixel(dpy, screen);
    window_attributes.border_pixel = XBlackPixel(dpy, screen);
    window_attributes.override_redirect = False;
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
    //setStringProperty("WM_NAME", "XTest");
    XSelectInput(dpy, window, ExposureMask|KeyPressMask|ButtonPressMask|ResizeRedirectMask|PointerMotionMask);
    XSetWMProtocols(dpy, window, &WM_DELETE_WINDOW, 1);

    XStoreName(dpy, window, TITLE);
    XTextProperty *iconproperty = malloc(sizeof(XTextProperty));
    int return_code;
    return_code = XStringListToTextProperty(&ICON_NAME,
                               1,
                               iconproperty);
    XSetWMIconName(dpy, window, iconproperty);
    free(iconproperty);

    XMapWindow(dpy, window);
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
    //sleep(1);

    while (1)  {
        XNextEvent(dpy, &events);
        switch  (events.type) {
            case Expose:
                initCairo();
                renderAll();
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
            case ResizeRequest:
                renderAll();
            break;
                       
        }
    }
    end:
    cleanup();
    return 0;
}