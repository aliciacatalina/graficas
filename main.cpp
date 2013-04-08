//
//  main.cpp
//  Proyecto
//
//  Created by Alicia Gonzalez on 3/30/13.
//  Copyright (c) 2013 Alicia Gonzalez. All rights reserved.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PI 3.141592657
#define MAXSTARS 400
#define MAXPOS 10000
#define MAXWARP 10
#define MAXANGLES 6000

enum
{
    NORMAL = 0,
    WEIRD = 1,
    STREAK = 0,
    CIRCLE = 1
};


typedef struct _starRec
{
    GLint type;
    float x[2], y[2], z[2];
    float offsetX, offsetY, offsetR, rotation;
} starRec;

GLenum doubleBuffer;
GLint windW = 300, windH = 300;

GLenum flag = NORMAL;
GLint starCount = MAXSTARS / 2;
float speed = .50;
GLint nitro = 0;
starRec stars[MAXSTARS];
float sinTable[MAXANGLES];

float Sin(float angle)
{
    return (sinTable[(GLint) angle]);
}

float Cos(float angle)
{
    return (sinTable[((GLint) angle + (MAXANGLES / 4)) % MAXANGLES]);
}

void NewStar(GLint n, GLint d)
{
    if (rand() % 4 == 0)
    {
        stars[n].type = CIRCLE;
    }
    else
    {
        stars[n].type = STREAK;
    }
    stars[n].x[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
    stars[n].y[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
    stars[n].z[0] = (float) (rand() % MAXPOS + d);
    stars[n].x[1] = stars[n].x[0];
    stars[n].y[1] = stars[n].y[0];
    stars[n].z[1] = stars[n].z[0];
    if (rand() % 4 == 0 && flag == WEIRD)
    {
        stars[n].offsetX = (float) (rand() % 100 - 100 / 2);
        stars[n].offsetY = (float) (rand() % 100 - 100 / 2);
        stars[n].offsetR = (float) (rand() % 25 - 25 / 2);
    }
    else
    {
        stars[n].offsetX = 0.0;
        stars[n].offsetY = 0.0;
        stars[n].offsetR = 0.0;
    }
}

void RotatePoint(float *x, float *y, float rotation)
{
    float tmpX, tmpY;
    
    tmpX = *x * Cos(rotation) - *y * Sin(rotation);
    tmpY = *y * Cos(rotation) + *x * Sin(rotation);
    *x = tmpX;
    *y = tmpY;
}

void MoveStars(void)
{
    float offset;
    GLint n;
    
    offset = speed * 60.0;
    
    for (n = 0; n < starCount; n++)
    {
        stars[n].x[1] = stars[n].x[0];
        stars[n].y[1] = stars[n].y[0];
        stars[n].z[1] = stars[n].z[0];
        stars[n].x[0] += stars[n].offsetX;
        stars[n].y[0] += stars[n].offsetY;
        stars[n].z[0] -= offset;
        stars[n].rotation += stars[n].offsetR;
        if (stars[n].rotation > MAXANGLES)
        {
            stars[n].rotation = 0.0;
        }
    }
}

GLenum StarPoint(GLint n)
{
    float x0, y0;
    
    x0 = stars[n].x[0] * windW / stars[n].z[0];
    y0 = stars[n].y[0] * windH / stars[n].z[0];
    RotatePoint(&x0, &y0, stars[n].rotation);
    x0 += windW / 2.0;
    y0 += windH / 2.0;
    
    if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH)
    {
        return GL_TRUE;
    }
    else
    {
        return GL_FALSE;
    }
}

void ShowStar(GLint n)
{
    float x0, y0, x1, y1, width;
    GLint i;
    
    x0 = stars[n].x[0] * windW / stars[n].z[0];
    y0 = stars[n].y[0] * windH / stars[n].z[0];
    RotatePoint(&x0, &y0, stars[n].rotation);
    x0 += windW / 2.0;
    y0 += windH / 2.0;
    
    if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH)
    {
        if (stars[n].type == STREAK)
        {
            x1 = stars[n].x[1] * windW / stars[n].z[1];
            y1 = stars[n].y[1] * windH / stars[n].z[1];
            RotatePoint(&x1, &y1, stars[n].rotation);
            x1 += windW / 2.0;
            y1 += windH / 2.0;
            
            glLineWidth(MAXPOS / 100.0 / stars[n].z[0] + 1.0);
            glColor3f(1.0, (MAXWARP - speed) / MAXWARP, (MAXWARP - speed) / MAXWARP);
            if (fabs(x0 - x1) < 1.0 && fabs(y0 - y1) < 1.0)
            {
                glBegin(GL_POINTS);
                glVertex2f(x0, y0);
                glEnd();
            }
            else
            {
                glBegin(GL_LINES);
                glVertex2f(x0, y0);
                glVertex2f(x1, y1);
                glEnd();
            }
        }
        else
        {
            width = MAXPOS / 10.0 / stars[n].z[0] + 1.0;
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            for (i = 0; i < 8; i++)
            {
                float x = x0 + width * Cos((float) i * MAXANGLES / 8.0);
                float y = y0 + width * Sin((float) i * MAXANGLES / 8.0);
                glVertex2f(x, y);
            };
            glEnd();
        }
    }
}

void UpdateStars(void)
{
    GLint n;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (n = 0; n < starCount; n++)
    {
        if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP))
        {
            if (StarPoint(n) == GL_FALSE)
            {
                NewStar(n, MAXPOS);
            }
        }
        else
        {
            NewStar(n, MAXPOS);
        }
    }
}

void ShowStars(void)
{
    GLint n;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (n = 0; n < starCount; n++)
    {
        if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP))
        {
            ShowStar(n);
        }
    }
}
void myinit(void)
{
    
    float angle;
    GLint n;
    
    srand((unsigned int) time(NULL));
    
    for (n = 0; n < MAXSTARS; n++)
    {
        NewStar(n, 100);
    }
    
    angle = 0.0;
    for (n = 0; n <= MAXANGLES; n++)
    {
        sinTable[n] = Sin(angle);
        angle += PI / (MAXANGLES / 2.0);
    }
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    

}
void Idle(void)
{
    MoveStars();
    UpdateStars();
    if (nitro > 0)
    {
        speed = (float) (nitro / 10) + 1.0;
        if (speed > MAXWARP)
        {
            speed = MAXWARP;
        }
        if (++nitro > MAXWARP * 10)
        {
            nitro = -nitro;
        }
    }
    else if (nitro < 0)
    {
        nitro++;
        speed = (float) (-nitro / 10) + 1.0;
        if (speed > MAXWARP)
        {
            speed = MAXWARP;
        }
    }
    glutPostRedisplay();
}

void display(void)
{
    ShowStars();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  glMatrixMode(GL_MODELVIEW);
    //estrellitas
    glPointSize(5.0);
    for(int i = 0; i<=30; i++){
        glBegin(GL_POINTS);
        glColor3f((double)rand()/(RAND_MAX), (double)rand()/(RAND_MAX), (double)rand()/(RAND_MAX));
        glVertex3f(((rand()%400)-200), ((rand()%400)-200), 100.0);
        glEnd();
    }
    
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(10.0);
    //Camino de arriba
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 10.0f, 0.0f);
    glVertex3f(-200.0f, 200.0f, 0.0f);
    glVertex3f(200.0f, 200.0f, 0.0f);
    glVertex3f(10.0f, 10.0f, 0.0f);
    glEnd();
    //Camino de abajo
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -10.0f, 0.0f);
    glVertex3f(-200.0f, -200.0f, 0.0f);
    glVertex3f(200.0f, -200.0f, 0.0f);
    glVertex3f(10.0f, -10.0f, 0.0f);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(11.0, -10.0, 0.0);
    glVertex3f(101.0, -200.0, 0.0);
    glEnd();
    
    glFlush();
}


void reshape (int width, int height)
{
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    glOrtho(200, -200.0, -200, 200, 200, -200 );
//    gluPerspective(45.0f, aspect, 1.0f, 200.0f);
    glFrustum(-200, 200, -200, 200, 1.0, 200.0);
    gluLookAt(0, 0, 6, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void key(unsigned char k, int x, int y)
{
    switch (k) {
        case 27:  /* Escape */
            exit(0);
            break;
        default:
            return;
    }
}
void Visible(int state)
{
    if (state == GLUT_VISIBLE)
    {
        glutIdleFunc(Idle);
    }
    else
    {
        glutIdleFunc(NULL);
    }
}

static void Args(int argc, char **argv)
{
    GLint i;
    
    doubleBuffer = GL_TRUE;
    
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-sb") == 0)
        {
            doubleBuffer = GL_FALSE;
        }
        else if (strcmp(argv[i], "-db") == 0)
        {
            doubleBuffer = GL_TRUE;
        }
    }
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Proyecto Final");
    myinit();
    glEnable(GL_DEPTH_TEST);
    glutReshapeFunc(reshape);
    glutVisibilityFunc(Visible);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;             /* ANSI C requires main to return int. */
}
