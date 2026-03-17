#ifndef SIM_H
#define SIM_H

#define TRACE_MAX 8096
#define G 6.674e+4
#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>

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

void updateBody(Body *b, double dt);
void updateAcc(Body *b);
void updateForce(Body *b1, Body* b2);
double getDeltaTime(struct timespec *last);
void recordTrace(Body *b);

#endif