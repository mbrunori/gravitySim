#include "sim.h"
#include "render.h"

#include <math.h>
#include <stdio.h>

#define width 800
#define height 600



int main(int argc, char const *argv[])
{
    //init display
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Errore apertura display\n");
        return 1;
    }

    int screen = DefaultScreen(display);

    //init window
    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100, width, height, 1,
        WhitePixel(display, screen),
        BlackPixel(display, screen)
    );

    XSelectInput(
        display, window,
        ExposureMask |
        KeyPressMask |
        StructureNotifyMask
    );

    //show window
    XMapWindow(display, window);

    //init GC
    GC gc = XCreateGC(display, window, 0, NULL);

    Colormap colormap = DefaultColormap(display, screen);
    XColor yellow;
    XParseColor(display, colormap, "yellow", &yellow);
    XAllocColor(display, colormap, &yellow);
    XColor black;
    XParseColor(display, colormap, "black", &black);
    XAllocColor(display, colormap, &black);
    
    XSetBackground(display, gc, black.pixel);
    XSetForeground(display, gc, yellow.pixel);

    //body initializations
    Body b1 = { {600,300}, {0,-100}, {0,0}, {0,0}, 1.0, 25.0};
    Body b2 = { {400,300}, {0,0}, {0,0}, {0,0}, 100.0, 25.0};

    //clock setup
    int running = 1;
    struct timespec lastTime;
    clock_gettime(CLOCK_MONOTONIC, &lastTime);

    while (running) {

        // Handle pending X events
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            switch(event.type) {
                case KeyPress:
                    running = 0;
                    break;
                case DestroyNotify:
                    running = 0;
                    break;
            }
        }

        // Compute delta time
        double dt = getDeltaTime(&lastTime);

        // Update physics
        updateForce(&b1, &b2);
        updateAcc(&b1);
        updateBody(&b1, dt);
        recordTrace(&b1);

        // Clear and draw
        XClearWindow(display, window);
        drawBody(display, window, gc, &b1);
        drawTrace(display, window, gc, &b1);
        drawBody(display, window, gc, &b2);

        // Limit framerate ~60 FPS
        struct timespec ts = {0, 16000000};
        nanosleep(&ts, NULL);
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    
    return 0;
}