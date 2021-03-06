// modified by: Nick Polach
// date: 01/26/17
// purpose: add collision to particles
//
//cs3350 Spring 2017 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

//#define MAX_PARTICLES 1 // For debugging
#define MAX_PARTICLES 10000
#define GRAVITY 0.15
#define rnd() (float)rand() / (float)RAND_MAX

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
        float x, y, z;
};

struct Shape {
        float width, height;
        float radius;
        Vec center;
};

struct Particle {
        Shape s;
        Vec velocity;
};

struct Game {
        Shape box[5];
        Shape circle;
        Particle particle[MAX_PARTICLES];
        int n;
        int bubbler;
        int mouse[2];
        Game() { n=0; bubbler=1; mouse[0]=0; mouse[1]=0; }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
void makeShapes(Game *game);


int main(void)
{
        int done=0;
        srand(time(NULL));
        initXWindows();
        init_opengl();
        //declare game object
        Game game;
        game.n=0;

        // set up boxes
        makeShapes(&game);

        //start animation
        while (!done) {
                while (XPending(dpy)) {
                        XEvent e;
                        XNextEvent(dpy, &e);
                        check_mouse(&e, &game);
                        done = check_keys(&e, &game);
                }
                movement(&game);
                render(&game);
                glXSwapBuffers(dpy, win);
        }
        cleanupXWindows();
        cleanup_fonts();
        return 0;
}

void set_title(void)
{
        //Set the window title bar.
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "3350 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
        //do not change
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
}

void initXWindows(void)
{
        //do not change
        GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
                std::cout << "\n\tcannot connect to X server\n" << std::endl;
                exit(EXIT_FAILURE);
        }
        Window root = DefaultRootWindow(dpy);
        XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL) {
                std::cout << "\n\tno appropriate visual found\n" << std::endl;
                exit(EXIT_FAILURE);
        } 
        Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                ButtonPress | ButtonReleaseMask | PointerMotionMask |
                StructureNotifyMask | SubstructureNotifyMask;
        win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
                        InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        set_title();
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
        //OpenGL initialization
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //Initialize matrices
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        //Set 2D mode (no perspective)
        glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
        //Set the screen background color
        glClearColor(0.1, 0.1, 0.1, 1.0);
        initialize_fonts();
}

void makeShapes (Game *game)
{
        // Requirements box
        game->box[0].width = 100;
        game->box[0].height = 10;
        game->box[0].center.x = 150;
        //game->box[0].center.y = 550;
        game->box[0].center.y = 550;

        // Design box
        game->box[1].width = 100;
        game->box[1].height = 10;
        game->box[1].center.x = 250;
        //game->box[1].center.y = 500;
        game->box[1].center.y = 450;

        // Coding box
        game->box[2].width = 100;
        game->box[2].height = 10;
        game->box[2].center.x = 350;
        //game->box[2].center.y = 450;
        game->box[2].center.y = 350;

        // Testing box
        game->box[3].width = 100;
        game->box[3].height = 10;
        game->box[3].center.x = 450;
        //game->box[3].center.y = 400;
        game->box[3].center.y = 250;

        // Maintenance box
        game->box[4].width = 100;
        game->box[4].height = 10;
        game->box[4].center.x = 550;
        //game->box[4].center.y = 350;
        game->box[4].center.y = 150;

        // Circle
        game->circle.radius = 125;
        game->circle.center.x = 800;
        game->circle.center.y = 0;
}

void makeParticle(Game *game, int x, int y)
{
        if (game->n >= MAX_PARTICLES)
                return;
        std::cout << "makeParticle() " << x << " " << y << std::endl;
        //position of particle
        Particle *p = &game->particle[game->n];
        p->s.center.x = x;
        p->s.center.y = y;
        // Adust below two lines to adjust initial velocities
        p->velocity.y = rnd() * 1 - 0.5;
        p->velocity.x = rnd() * 2 - 1;

        game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
        static int savex = 0;
        static int savey = 0;
        //	static int n = 0;

        if (e->type == ButtonRelease) {
                return;
        }
        if (e->type == ButtonPress) {
                if (e->xbutton.button==1) {
                        //Left button was pressed
                        // Below line because mouse coords are reverse (this unreverses them)
                        int y = WINDOW_HEIGHT - e->xbutton.y;
                        for (int i=0; i<10; i++) {
                                makeParticle(game, e->xbutton.x, y);
                        }
                        return;
                }
                if (e->xbutton.button==3) {
                        //Right button was pressed
                        return;
                }
        }
        //Did the mouse move?
        if (savex != e->xbutton.x || savey != e->xbutton.y) {
                savex = e->xbutton.x;
                savey = e->xbutton.y;
                int y = WINDOW_HEIGHT - e->xbutton.y;
                if (!game->bubbler) {
                        game->mouse[0] = savex;
                        game->mouse[1] = y;
                }
                for (int i=0; i<5; i++) {
                        makeParticle(game, e->xbutton.x, y);
                }
                //		if (++n < 10)
                //		    return;
        }
}

int check_keys(XEvent *e, Game *game)
{
        //Was there input from the keyboard?
        if (e->type == KeyPress) {
                int key = XLookupKeysym(&e->xkey, 0);
                if (key == XK_b) {
                        game->bubbler ^= 1;

                }
                if (key == XK_Escape) {
                        return 1;
                }
                //You may check other keys here.



        }
        return 0;
}

void movement(Game *game)
{
        Particle *p;

        if (game->n <= 0)
                return;

        if (game->bubbler != 0) {
                // the bubbler is on!
                for (int i = 0; i < 10; i++) {
                        // start bubbler above boxes
                        makeParticle(game, 150, 590);
                        // start bubbler at mouse
                        //makeParticle(game, game->mouse[0], game->mouse[1]);
                }
        }

        for (int i=0; i < game->n; i++) {
                p = &game->particle[i];
                p->velocity.y -= GRAVITY;
                p->s.center.x += p->velocity.x;
                p->s.center.y += p->velocity.y;

                //check for collision with boxes...
                Shape *s;
                for (int i=0; i < 5; i++) {
                        s = &game->box[i];
                        if (p->s.center.y < s->center.y + s->height && // top collision
                                        p->s.center.y > s->center.y - s->height && // bottom collision
                                        p->s.center.x >= s->center.x - s->width && // left collision
                                        p->s.center.x <= s->center.x + s->width) { // right collision
                                p->s.center.y = s->center.y + s->height;
                                p->velocity.y = -p->velocity.y * 0.005f;
                                //p->velocity.y = -p->velocity.y * 0.025f;
                                p->velocity.x += 0.02f;
                        }
                }

                //check for collision with circle...
                s = &game->circle;
                float dist;
                float xdiff, ydiff;
                float xnorm, ynorm;
                xdiff = p->s.center.x - s->center.x;
                ydiff = p->s.center.y - s->center.y;
                dist = sqrt((xdiff * xdiff) + (ydiff * ydiff)); 
                xnorm = xdiff / dist;
                ynorm = ydiff / dist;
                if (dist <= s->radius) {
                        // Move to edge of circle
                        p->s.center.x = s->center.x + (xnorm * s->radius);
                        p->s.center.y = s->center.y + (ynorm * s->radius);

                        // Add vector value to velocity
                        p->velocity.x += xnorm;
                        p->velocity.y += ynorm;
                }

                //check for off-screen
                if (p->s.center.y < 0.0) {
                        std::cout << "off screen" << std::endl;
                        // decrement n and assign last particle to current position
                        game->particle[i] = game->particle[--game->n];
                }
        }
}

void render(Game *game)
{
        float w, h;
        glClear(GL_COLOR_BUFFER_BIT);
        //Draw shapes...
        Shape *s;
        //draw box
        for (int i=0; i < 5; i++) {
                glColor3ub(90,140,90);
                s = &game->box[i];
                glPushMatrix();
                glTranslatef(s->center.x, s->center.y, s->center.z);
                w = s->width;
                h = s->height;
                glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
                glEnd();
                glPopMatrix();
        }

        //draw circle
        Shape *c = &game->circle;
        int triangleAmount = 50;	

        double pi = 3.1415926535897;
        GLfloat twicePi = 2.0f * pi;

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c->center.x, c->center.y);
        for (int i = 0; i<= triangleAmount; i++) {
                glVertex2f(
                                c->center.x + (c->radius * cos(i * twicePi / triangleAmount)),
                                c->center.y + (c->radius * sin(i * twicePi / triangleAmount)));
        }
        glEnd();
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
        glColor3ub(0,0,255);
        //draw all text here
        Rect r;
        r.bot = WINDOW_HEIGHT - 60;
        r.left = 100;
        r.center = 0;
        ggprint16(&r, 20, 0, "Requirements");
        r.bot = WINDOW_HEIGHT - 160;
        r.left = 220;
        ggprint16(&r, 20, 0, "Design");
        r.bot = WINDOW_HEIGHT - 260;
        r.left = 300;
        ggprint16(&r, 20, 0, "Implementation");
        r.bot = WINDOW_HEIGHT - 360;
        r.left = 410;
        ggprint16(&r, 20, 0, "Testing");
        r.bot = WINDOW_HEIGHT - 460;
        r.left = 510;
        ggprint16(&r, 20, 0, "Release");

        //draw all particles here
        for (int i=0; i < game->n; i++) {
                glPushMatrix();
                glColor3ub(80 + rnd() * 40 - 20, 100 + rnd() * 40 - 20,220 + rnd() * 70 - 35);
                Vec *c = &game->particle[i].s.center;
                w = 2;
                h = 2;
                glBegin(GL_QUADS);
                glVertex2i(c->x-w, c->y-h);
                glVertex2i(c->x-w, c->y+h);
                glVertex2i(c->x+w, c->y+h);
                glVertex2i(c->x+w, c->y-h);
                glEnd();
                glPopMatrix();
        }
}



