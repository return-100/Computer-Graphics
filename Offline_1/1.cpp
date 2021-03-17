#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

using namespace std;

#define pi (2 * acos(0.0))

const double del = 1.0;
const double max_cube_length = 20;
const double max_sphere_radius = 20;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double cube_length = 20;
double sphere_radius = 0;

struct point
{
    double x,y,z;
};

point p = {100, 100, 30};
point l = {-0.71, -0.71, 0};
point r = {-0.71, 0.71, 0};
point u = {0, 0, 1};

point add(point a, point b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

point sub(point a, point b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

point cross_product(point a, point b)
{
    point ret;

    ret.x = (b.y * a.z) - (b.z * a.y);
    ret.y = (b.z * a.x) - (b.x * a.z);
    ret.z = (b.x * a.y) - (b.y * a.x);

    return ret;
}

point rotate(point a, point b, double rotation_angle)
{
    point ret;
    point n = cross_product(a, b);

    ret.x = a.x * cos(rotation_angle) + n.x * sin(rotation_angle);
    ret.y = a.y * cos(rotation_angle) + n.y * sin(rotation_angle);
    ret.z = a.z * cos(rotation_angle) + n.z * sin(rotation_angle);

    return ret;
}

void drawAxes(int a)
{
    if(drawaxes==1)
    {
        glBegin(GL_LINES);
        {
            glColor3f(0, 0, 1.0);
            glVertex3f(a, 0, 0);
            glColor3f(0, 1.0, 0);
            glVertex3f(-a, 0, 0);

            glColor3f(1, 0, 0);
            glVertex3f(0, -a, 0);
            glColor3f(0, 1, 1);
            glVertex3f(0, a, 0);

            glColor3f(1, 0, 1);
            glVertex3f(0, 0, a);
            glColor3f(1, 1, 0);
            glVertex3f(0, 0, -a);
        }
        glEnd();
    }
}

void drawGrid()
{
    if(drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6);

        glBegin(GL_LINES);
        {
            for(int i=-8;i<=8;i++)
            {
                if(i == 0)
                    continue;

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    {
        glVertex3f( a, a, a);
        glVertex3f( a,-a, a);
        glVertex3f(-a,-a, a);
        glVertex3f(-a, a, a);
    }
    glEnd();
}

void drawCube()
{
    glPushMatrix();
    {
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 1, 0, 0);
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, -1, 0, 0);
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 1, 0);
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, -1, 0);
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 1, 0, 0);
        glRotated(90, 1, 0, 0);
        glTranslatef(0, 0, max_cube_length - cube_length);
        drawSquare(cube_length);
    }
    glPopMatrix();
}

void drawOneEighthOfSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];

    double h, r;

    for (int i = 0; i <= stacks; ++i)
    {
        h = radius * sin(((double) i / (double) stacks) * (pi / 2));
        r = radius * cos(((double) i / (double) stacks) * (pi / 2));

        for (int j = 0; j <= slices; ++j)
        {
            points[i][j].x = r * cos(((double) j / (double) slices) * (pi / 2));
            points[i][j].y = r * sin(((double) j / (double) slices) * (pi / 2));
            points[i][j].z = h;
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        glColor3f(1, 0, 0);

        for (int j = 0; j < slices; ++j)
        {
            glBegin(GL_QUADS);
            {
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j+1].x, points[i][j+1].y, points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, points[i+1][j].y, points[i+1][j].z);
            }
            glEnd();
        }
    }
}

void drawSphere(double radius, int slices, int stacks)
{
    glPushMatrix();
    {
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 0, -1);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(180, 0, 1, 0);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-90, 0, -1, 0);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-90, 0, 0, -1);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(180, 0, 0, -1);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(180, 1, 0, 0);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-180, 1, 0, -1);
        glTranslatef(cube_length, cube_length, cube_length);
        drawOneEighthOfSphere(radius, slices, stacks);
    }
    glPopMatrix();
}

void drawOneFourthOfCylinder(double radius, int height, int slices, int stacks)
{
    point points[100][100];

    for (int i = 0; i <= stacks; ++i)
    {
        for (int j = 0; j <= slices; ++j)
        {
            points[i][j].x = radius * (sin(((double) j / (double) slices) * .5 * pi));
            points[i][j].y = radius * (cos(((double) j / (double) slices) * .5 * pi));
            points[i][j].z = height * (sin(((double) i / (double) stacks) * .5 * pi));
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        glColor3f(0, 1, 0);

        for (int j = 0; j < slices; ++j)
        {
            glBegin(GL_QUADS);
            {
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j+1].x, points[i][j+1].y, points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, points[i+1][j].y, points[i+1][j].z);

                glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                glVertex3f(points[i][j+1].x, points[i][j+1].y, -points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, -points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, points[i+1][j].y, -points[i+1][j].z);
            }
            glEnd();
        }
    }
}

void drawCylinder(double radius, int height, int slices, int stacks)
{
    glPushMatrix();
    {
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 0, 1);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 0, -1);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(180, 0, 0, -1);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90, 1, 0, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-90, 1, 0, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 0, 1);
        glRotatef(90, 0, -1, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90, 0, 1, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-90, 0, 1, 0);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(90, 0, 1, 0);
        glRotatef(90, 0, 0, -1);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotated(-90, 0, 1, 0);
        glRotatef(-90, 0, 0, 1);
        glTranslatef(cube_length, cube_length, 0);
        drawOneFourthOfCylinder(radius, height, slices, stacks);
    }
    glPopMatrix();
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '1':
            l = rotate(l, u, 0.1);
            r = rotate(r, u, 0.1);
            break;

        case '2':
            l = rotate(l, u, -0.1);
            r = rotate(r, u, -0.1);
            break;

        case '3':
            l = rotate(l, r, 0.1);
            u = rotate(u, r, 0.1);
            break;

        case '4':
            l = rotate(l, r, -0.1);
            u = rotate(u, r, -0.1);
            break;

        case '5':
            r = rotate(r, l, 0.1);
            u = rotate(u, l, 0.1);
            break;

        case '6':
            r = rotate(r, l, -0.1);
            u = rotate(u, l, -0.1);
            break;

        default:
            break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_DOWN:
            p = sub(p, l);
            break;

        case GLUT_KEY_UP:
            p = add(p, l);
            break;

        case GLUT_KEY_RIGHT:
            p = add(p, r);
            break;

        case GLUT_KEY_LEFT:
            p = sub(p, r);
            break;

        case GLUT_KEY_PAGE_UP:
            p = add(p, u);
            break;

        case GLUT_KEY_PAGE_DOWN:
            p = sub(p, u);
            break;

        case GLUT_KEY_INSERT:
            break;

        case GLUT_KEY_HOME:
            if (cube_length > 0.0)
                cube_length -= del;

            if (sphere_radius < max_sphere_radius)
                sphere_radius += del;
            break;

        case GLUT_KEY_END:
            if (cube_length < max_cube_length)
                cube_length += del;

            if (sphere_radius > 0.0)
                sphere_radius -= del;
            break;

        default:
            break;
    }
}

void mouseListener(int button, int state, int x, int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN)
                drawaxes=1-drawaxes;
            break;

        case GLUT_RIGHT_BUTTON:
            break;

        case GLUT_MIDDLE_BUTTON:
            break;

        default:
            break;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    gluLookAt(p.x, p.y, p.z, p.x + l.x, p.y + l.y, p.z + l.z, u.x, u.y, u.z);

    glMatrixMode(GL_MODELVIEW);

    drawAxes(100);
    drawCube();
    drawSphere(sphere_radius, 50, 50);
    drawCylinder(sphere_radius, cube_length, 50, 50);

    glutSwapBuffers();
}

void animate()
{
    angle += 1;
    glutPostRedisplay();
}

void init()
{
    drawgrid=1;
    drawaxes=1;
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;

    glClearColor(0,0,0,0);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(80,    1,    1,    1000.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

    glutCreateWindow("1");

    init();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutIdleFunc(animate);

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();

    return 0;
}

