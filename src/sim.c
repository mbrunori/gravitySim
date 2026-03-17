#include "sim.h"

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

void recordTrace(Body *b)
{
    if (b->traceCount < TRACE_MAX) {
        b->trace[b->traceCount++] = b->pos;
    }
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