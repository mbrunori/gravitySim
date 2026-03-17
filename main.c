#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <X11/Xlib.h>

#define width 800
#define height 600

#define TRACE_MAX 8096

#define G 6.674e+4

typedef struct {
    double x;
    double y;
}Vector2;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;

    Vector2 force;

    double mass;
    double radius;

    Vector2 trace[TRACE_MAX];
    int traceCount;
} Body;

void drawBody(Display *display, Window window, GC gc, Body *b);
double getDeltaTime(struct timespec *last);
void updateBody(Body *b, double dt);
void updateAcc(Body *b);
void updateForce(Body *b1, Body* b2);
void drawTrace(Display *display, Window window, GC gc, Body *b);
void recordTrace(Body *b);

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


void recordTrace(Body *b)
{
    if (b->traceCount < TRACE_MAX) {
        b->trace[b->traceCount++] = b->pos;
    }
}

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

double getDeltaTime(struct timespec *last) 
{
    struct timespec current;
    clock_gettime(CLOCK_MONOTONIC, &current);

    double delta = (current.tv_sec - last->tv_sec)
                 + (current.tv_nsec - last->tv_nsec) / 1e9;

    *last = current; // update last time
    return delta;
}

void updateForce(Body *b1, Body* b2)
{
    double dx = b2->pos.x - b1->pos.x;
    double dy = b2->pos.y - b1->pos.y;

    double sqrDist = dx*dx + dy*dy + 1e-6;  // prevent division by zero
    double dist = sqrt(sqrDist);

    double force = (G * b1->mass * b2->mass) / sqrDist;

    // Normalized direction vector
    double fx = force * dx / dist;
    double fy = force * dy / dist;

    // Apply equal and opposite forces
    b1->force.x = fx;
    b1->force.y = fy;

    b2->force.x = -fx;
    b2->force.y = -fy;
}

void updateAcc(Body *b)
{
    b->acc.x = b->force.x / b->mass;
    b->acc.y = b->force.y / b->mass;
}

void updateBody(Body *b, double dt)
{
    //update velocity
    b->vel.x += b->acc.x * dt;
    b->vel.y += b->acc.y * dt;

    //update positions
    b->pos.x += b->vel.x * dt;
    b->pos.y += b->vel.y * dt;
}