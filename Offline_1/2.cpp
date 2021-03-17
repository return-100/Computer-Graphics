#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

using namespace std;

#define pi (2 * acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double height = 8;
double radius = 20;
double del = 0;
double ang = 0;
double del_color = 0.1;

struct point
{
    double x,y,z;
};

point center_pos;
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

void drawRectangle(double x, double y, double z)
{
    glColor3f(0.5, 0.5, 0.5);

    glBegin(GL_QUADS);
    {
        glVertex3f(x, y, z);
        glVertex3f(x, -y, z);
        glVertex3f(-x, -y, z);
        glVertex3f(-x, y, z);
    }
    glEnd();
}

void drawCylinder(double radius, int height, int slices, int stacks)
{
    point points[100][100];

    for (int i = 0; i <= stacks; ++i)
    {
        for (int j = 0; j <= slices; ++j)
        {
            points[i][j].x = height * (sin(((double) i / (double) stacks) * 2 * pi));
            points[i][j].y = radius * (cos(((double) j / (double) slices) * 2 * pi));
            points[i][j].z = radius + radius * (sin(((double) j / (double) slices) * 2 * pi));
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            double shade_val = (double) j / (double) slices;

            glColor3f(1.1 - shade_val, 1.1 - shade_val, 1.1 - shade_val);

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

        case 'w':
            center_pos.x += 2 * sin(-ang * pi / 180);
            center_pos.y += 2 * cos(-ang * pi / 180);
            del += 2;
            break;

        case 's':
            center_pos.x -= 2 * sin(-ang * pi / 180);
            center_pos.y -= 2 * cos(-ang * pi / 180);
            del -= 2;
            break;

        case 'd':
            ang -= 1;
            break;

        case 'a':
            ang += 1;
            break;

        default:
            break;
    }
}

void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
        case GLUT_KEY_DOWN:
            cameraHeight -= 3.0;
            break;

        case GLUT_KEY_UP:
            cameraHeight += 3.0;
            break;

        case GLUT_KEY_RIGHT:
            cameraAngle += 0.03;
            break;

        case GLUT_KEY_LEFT:
            cameraAngle -= 0.03;
            break;

        case GLUT_KEY_PAGE_UP:
            break;

        case GLUT_KEY_PAGE_DOWN:
            break;

        case GLUT_KEY_INSERT:
            break;

        case GLUT_KEY_HOME:
            break;

        case GLUT_KEY_END:
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

    gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,     0,0,0,      0,0,1);

    glMatrixMode(GL_MODELVIEW);

    drawAxes(100);
    drawGrid();


        glTranslatef(center_pos.x, center_pos.y, 0);
        glRotatef(ang, 0, 0, 1);
        glTranslatef(0, 0, radius);
        glRotatef(-360 * del / (2.0 * pi * radius), 1, 0, 0);
        glTranslatef(0, 0, -radius);
        drawCylinder(radius, height, 40, 40);
        drawRectangle(height, radius, radius);

        glTranslatef(0, radius, radius);
        glRotatef(90, 1, 0, 0);
        drawRectangle(height, radius, radius);



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
    drawaxes=0;
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

    glutCreateWindow("2");

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
