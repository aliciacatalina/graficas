//#include<windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "fmod.h"
#include "fmod_errors.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.141592657
#define MAXSTARS 400
#define MAXPOS 10000
#define MAXWARP 10
#define MAXANGLES 6000
static GLfloat zPost = 0.0f;
double posGame = -0.7;
char game[] = {'G','A','M','E','O','V','E','R'};
GLfloat collisionsX[] = {-150.0, //primer nivel
                        -50.0,
                        50.0,
                        150.0,
                        -150.0,//segundo nivel
                        -50.0,
                        50.0,
                        150.0
                        -150.0,//tercer nivel
                        -50.0,
                        50.0,
                        150.0};
GLfloat collisionsZ[] = {1.0, 1.0, 1.0, 1.0, -2.0, -2.0, -2.0, -2.0, -8.0, -8.0, -8.0, -8.0};
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

GLfloat triangle_vertices[] =
{
    
    -20.0,-255.0,0.0,
    20.0, -255.0,0.0,
    0.0,  -200.0,0.0,
};

struct
{
    struct
    {
        struct
        {
            GLfloat x, y, z;
        } pos;
        GLfloat rot;
    } triangle;
} sceneinfo;

GLenum doubleBuffer;
GLint windW = 500, windH = 500;

GLenum flag = NORMAL;
GLint starCount = MAXSTARS / 2;
float speed = 1.0;
GLint nitro = 0;
starRec stars[MAXSTARS];
float sinTable[MAXANGLES];

void timer(int value)
{
    zPost = zPost + .50;
    //zTiles = zTiles + .10;
    for(int i =0; i<=12;i++){
        if(sceneinfo.triangle.pos.x == collisionsX[i] && zPost == collisionsZ[i]){
            std::cout << "zpost ";
            std::cout << zPost;
            std::cout << "posarray ";
            std::cout << collisionsZ[i];
            std::cout << "collision";
        }
    }
    glutPostRedisplay();
    glutTimerFunc(300,timer,0);
    
    
}
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
    
    if ((x0 >= (-windW)) && x0 < windW && (y0 >= (-windH)) && y0 < windH)
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
    
    x0 = stars[n].x[0] * windW / stars[n].z[0]-250;
    y0 = stars[n].y[0] * windH / stars[n].z[0]-250;
    RotatePoint(&x0, &y0, stars[n].rotation);
    x0 += windW / 2.0;
    y0 += windH / 2.0;
    
    if (x0 >= (-windW) && x0 < windW && y0 >= (-windH) && y0 < windH)
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
            if ((x0 - x1) < -1.0 && (y0 - y1) < -1.0)
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

static void Init(void)
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
        sinTable[n] = sin(angle);
        angle += PI / (MAXANGLES / 2.0);
    }
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glDisable(GL_DITHER);
}

void Reshape(int width, int height)
{
    windW = width;
    windH = height;
    
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-200, 200, -200, 200, 1.0, 2000.0);
    gluLookAt(0, 0, 1.3, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ARGSUSED1 */
static void Key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case ' ':
            flag = (flag == NORMAL) ? WEIRD : NORMAL;
            break;
        case 't':
            nitro = 1;
            break;
        case 27:
            exit(0);
    }
}
void special(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_LEFT:
            sceneinfo.triangle.pos.x -= 5.0;
            break;
        case GLUT_KEY_RIGHT:
            sceneinfo.triangle.pos.x += 5.0;
            break;
    }
    glutPostRedisplay();
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

void GameOver(void){
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);
    for(int i=0; i<8; i++) {
        glRasterPos3f(posGame,0.0, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game[i]);
        posGame+=0.2;
    }
        glPopMatrix();
//        for(int i = 0; i<=12; i++){
//            if(sceneinfo.triangle.pos.x == collisionsX[i] && zPost == collisionsZ[i]){
//            std::cout << "zpost ";
//            std::cout << zPost;
//            std::cout << "posarray ";
//            std::cout << collisionsZ[i];
//            //std::cout << "collision";
//            //GameOver();
//             }
        //}
}
void Display(void)
{
    ShowStars();
    GameOver();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, zPost);
    //Camino de arriba
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(-200.0f, 200.0f, -1000.0f);
    glVertex3f(-200.0f, 200.0f, 1.0f);
    glVertex3f(200.0f, 200.0f, 1.0f);
    glVertex3f(200.0f, 200.0f, -1000.0f);
    glEnd();
    //Camino de abajo
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(-200.0f, -200.0f, -1000.0f);
    glVertex3f(-200.0f, -200.0f, 1.0f);
    glVertex3f(200.0f, -200.0f, 1.0f);
    glVertex3f(200.0f, -200.0f, -1000.0f);
    glEnd();
        
    //lineas de camino arriba
    glLineWidth(3.0);
    glBegin(GL_LINES);
    for(float i = -200; i<=200; i+=50){
        glColor3f(0.64, 0.64, 0.64);
        glVertex3f(i, 200, -1000.0);
        glVertex3f(i, 200, 1.0);
    }
    glEnd();
    //lineas de camino abajo
    glLineWidth(3.0);
    glBegin(GL_LINES);
    for(float i = -200; i<=200; i+=50){
        glColor3f(0.64, 0.64, 0.64);
        glVertex3f(i, -200, -1000.0);
        glVertex3f(i, -200, 1.0);
    }
    glEnd();
    glPopMatrix();
    //mosaico primer nivel
    glPushMatrix();
    glTranslatef(0.0, 0.0, zPost);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_QUADS);
    glVertex3f(-200.0f, -195.0f, -1.0f);
    glVertex3f(-200.0f, -190.0f, 1.0f);
    glVertex3f(-150.0f, -190.0f, 1.0f);
    glVertex3f(-150.0f, -195.0f, -1.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -195.0f, -1.0f);
    glVertex3f(-100.0f, -190.0f, 1.0f);
    glVertex3f(-50.0f, -190.0f, 1.0f);
    glVertex3f(-50.0f, -195.0f, -1.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(50.0f, -195.0f, -1.0f);
    glVertex3f(50.0f, -190.0f, 1.0f);
    glVertex3f(100.0f, -190.0f, 1.0f);
    glVertex3f(100.0f, -195.0f, -1.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(150.0f, -195.0f, -1.0f);
    glVertex3f(150.0f, -190.0f, 1.0f);
    glVertex3f(200.0f, -190.0f, 1.0f);
    glVertex3f(200.0f, -195.0f, -1.0f);
    glEnd();
    //mosaico segundo nivel
    glBegin(GL_QUADS);
    glVertex3f(-200.0f, -195.0f, -4.0f);
    glVertex3f(-200.0f, -190.0f, -2.0f);
    glVertex3f(-150.0f, -190.0f, -2.0f);
    glVertex3f(-150.0f, -195.0f, -4.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -195.0f, -4.0f);
    glVertex3f(-100.0f, -190.0f, -2.0f);
    glVertex3f(-50.0f, -190.0f, -2.0f);
    glVertex3f(-50.0f, -195.0f, -4.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(50.0f, -195.0f, -4.0f);
    glVertex3f(50.0f, -190.0f, -2.0f);
    glVertex3f(100.0f, -190.0f, -2.0f);
    glVertex3f(100.0f, -195.0f, -4.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(150.0f, -195.0f, -4.0f);
    glVertex3f(150.0f, -190.0f, -2.0f);
    glVertex3f(200.0f, -190.0f, -2.0f);
    glVertex3f(200.0f, -195.0f, -4.0f);
    glEnd();
    
    //mosaicos tercer nivel
    glBegin(GL_QUADS);
    glVertex3f(-200.0f, -195.0f, -10.0f);
    glVertex3f(-200.0f, -190.0f, -8.0f);
    glVertex3f(-150.0f, -190.0f, -8.0f);
    glVertex3f(-150.0f, -195.0f, -10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -195.0f, -10.0f);
    glVertex3f(-100.0f, -190.0f, -8.0f);
    glVertex3f(-50.0f, -190.0f, -8.0f);
    glVertex3f(-50.0f, -195.0f, -10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(50.0f, -195.0f, -10.0f);
    glVertex3f(50.0f, -190.0f, -8.0f);
    glVertex3f(100.0f, -190.0f, -8.0f);
    glVertex3f(100.0f, -195.0f, -10.0f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(150.0f, -195.0f, -10.0f);
    glVertex3f(150.0f, -190.0f, -8.0f);
    glVertex3f(200.0f, -190.0f, -8.0f);
    glVertex3f(200.0f, -195.0f, -10.0f);
    glEnd();
    //mosaico color
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(-150.0f, -155.0f, -1.0f);
    glVertex3f(-150.0f, -150.0f, 1.0f);
    glVertex3f(-100.0f, -150.0f, 1.0f);
    glVertex3f(-100.0f, -155.0f, -1.0f);
    glEnd();
    
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(-50.0f, -155.0f, -1.0f);
    glVertex3f(-50.0f, -150.0f, 1.0f);
    glVertex3f(0.0f, -150.0f, 1.0f);
    glVertex3f(0.0f, -155.0f, -1.0f);
    glEnd();
    
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(100.0f, -155.0f, -1.0f);
    glVertex3f(100.0f, -150.0f, 1.0f);
    glVertex3f(150.0f, -150.0f, 1.0f);
    glVertex3f(150.0f, -155.0f, -1.0f);
    glEnd();

    glPopMatrix();
    
    glPushMatrix();
    
    glTranslatef(sceneinfo.triangle.pos.x, sceneinfo.triangle.pos.y, 0.);
    glRotatef(sceneinfo.triangle.rot, 0, 0, 0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triangle_vertices);
    glColor3f(1.0,0.0,0.0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glPopMatrix();

    glPopMatrix();
//    std::cout << "X=";
//    std::cout << sceneinfo.triangle.pos.x;
//    std::cout << ",";
//    std::cout << "Y=";
//    std::cout << sceneinfo.triangle.pos.y;
//    std::cout << ",";
//    std::cout << "Z=";
//    std::cout << sceneinfo.triangle.pos.z;
    glutSwapBuffers();
    
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
    
    
    GLenum type;
    
    glutInitWindowSize(windW, windH);
    glutInit(&argc, argv);
    Args(argc, argv);
    
    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    
    glutCreateWindow("Stars");
    
    Init();
    
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(special);
    glutVisibilityFunc(Visible);
    glutDisplayFunc(Display);
    glutTimerFunc(100,timer,0);
    glutMainLoop();
    return 0;
}
