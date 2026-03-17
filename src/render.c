#include "render.h"

void drawTrace(Display *display, Window window, GC gc, Body *b)
{
    for (int i = 0; i < b->traceCount; i++) {
        XFillRectangle(
            display,
            window,
            gc,
            (int)b->trace[i].x,
            (int)b->trace[i].y,
            2,
            2
        );
    }
}

void drawBody(Display *display, Window window, GC gc, Body *b)
{
    int diameter = (int)(b->radius * 2);

    int drawX = (int)(b->pos.x - b->radius);
    int drawY = (int)(b->pos.y - b->radius);

    XFillArc(display,
        window,
        gc,
        drawX,
        drawY,
        diameter,
        diameter,
        0,
        360*64
    );
}