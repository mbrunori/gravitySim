#ifndef RENDER_H
#define RENDER_H

#include <X11/Xlib.h>
#include "sim.h"

#define width 800
#define height 600

void drawTrace(Display *display, Window window, GC gc, Body *b);
void drawBody(Display *display, Window window, GC gc, Body *b);

#endif