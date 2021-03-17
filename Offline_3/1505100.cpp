#include <bits/stdc++.h>
#include "bitmap_image.hpp"

#include <windows.h>
#include <glut.h>

using namespace std;

#define pi (2 * acos(0.0))

const double del = 1.0;
const int window_height = 500;
const int window_width = 500;

int recursion_level;
int pixel_size;
int number_of_object;
int number_of_light;
int number_of_sphere;
int number_of_pyramid;

int drawgrid;
int drawaxes;
double angle, fovY;
double cameraHeight;
double cameraAngle;

string object_type;

class point
{
public:

    double x, y, z;

    double value()
    {
        return (x * x + y * y + z * z);
    }

    void normalize()
    {
        x /= value(), y /= value(), z /= value();
    }

    void scalar_multiplication(double val)
    {
        x *= val, y *= val, z *= val;
    }
};

class ray
{
public:

    point start, dir;

    ray()
    {

    }

    ray(point s, point d)
    {
        d.normalize();
        start = s, dir = d;
    }
};

point p = {100, 90, 30};
point l = {-(1 / sqrt(2)), -(1 / sqrt(2)), 0};
point r = {-(1 / sqrt(2)), (1 / sqrt(2)), 0};
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

double dot_product(point a, point b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
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

class sphere
{
public:

    double radius;
    point center;
    double color[3];
    double specular_exponent;
    double ambient_component;
    double diffuse_component;
    double specular_component;
    double reflection_component;

    void draw()
    {
        for (int i = 0; i < number_of_sphere; ++i)
        {
            glPushMatrix();
            {
                glColor3d(color[0], color[1], color[2]);
                glTranslated(center.x, center.y, center.z);
                gluSphere(gluNewQuadric(), radius, 100, 100);
            }
            glPopMatrix();
        }
    }

    double intersect_point(ray r)
    {
        point rs = sub(r.start, center), rd = r.dir;

        double a = 1.0;
        double b = 2 * dot_product(rs, rd);
        double c = dot_product(rs, rs) - (radius * radius);

        double d = (b * b) - (4 * a * c);

        if(d < 0)
            return -1;

        d = sqrt(d);
        double t1 = (- b - d) / (2 * a);
        double t2 = (- b + d) / (2 * a);

        return min(t1, t2);
    }
};

class pyramid
{
public:

    double color[3];
    double base, height;
    point lowest_coordinate;
    double specular_exponent;
    double ambient_component;
    double diffuse_component;
    double specular_component;
    double reflection_component;
    point triangle[6][3];

    void generateTriangle()
    {
        point top;

        top.x = (2 * lowest_coordinate.x + base) / 2.0;
        top.y = (2 * lowest_coordinate.y + base) / 2.0;
        top.z = lowest_coordinate.z + height;

        for (int i = 2; i < 6; ++i)
            triangle[i][1] = top;

        triangle[0][0] = lowest_coordinate;
        triangle[0][1].x = lowest_coordinate.x + base, triangle[0][1].y = lowest_coordinate.y, triangle[0][1].z = lowest_coordinate.z;
        triangle[0][2].x = lowest_coordinate.x, triangle[0][2].y = lowest_coordinate.y + base, triangle[0][2].z = lowest_coordinate.z;

        triangle[1][0] = triangle[0][1];
        triangle[1][1].x = lowest_coordinate.x + base, triangle[1][1].y = lowest_coordinate.y + base, triangle[1][1].z = lowest_coordinate.z;
        triangle[1][2] = triangle[0][2];

        triangle[2][0] = triangle[0][1];
        triangle[2][2] = triangle[0][0];

        triangle[3][0] = triangle[1][1];
        triangle[3][2] = triangle[0][1];

        triangle[4][0] = triangle[0][2];
        triangle[4][2] = triangle[1][1];

        triangle[5][0] = triangle[0][0];
        triangle[5][2] = triangle[0][2];
    }

    void draw()
    {
        for (int i = 0; i < 6; ++i)
        {
            glPushMatrix();
            {
                glColor3d(color[0], color[1], color[2]);

                glBegin(GL_TRIANGLES);
                {
                    glVertex3d(triangle[i][0].x, triangle[i][0].y, triangle[i][0].z);
                    glVertex3d(triangle[i][1].x, triangle[i][1].y, triangle[i][1].z);
                    glVertex3d(triangle[i][2].x, triangle[i][2].y, triangle[i][2].z);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
};

vector <point> lights;
vector <sphere> sphere_arr;
vector <pyramid> pyramid_arr;

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

void read_input()
{
    ifstream in("in.txt");

    in >> recursion_level >> pixel_size >> number_of_object;

    for (int i = 0; i < number_of_object; ++i)
    {
        in >> object_type;

        if (object_type == "sphere" || object_type == "1")
        {
            ++number_of_sphere;
            sphere ob;

            in >> ob.center.x >> ob.center.y >> ob.center.z;
            in >> ob.radius;
            in >> ob.color[0] >> ob.color[1] >> ob.color[2];
            in >> ob.ambient_component >> ob.diffuse_component >> ob.specular_component >> ob.reflection_component;
            in >> ob.specular_exponent;

            sphere_arr.push_back(ob);
        }
        else
        {
            ++number_of_pyramid;
            pyramid ob;

            in >> ob.lowest_coordinate.x >> ob.lowest_coordinate.y >> ob.lowest_coordinate.z;
            in >> ob.base >> ob.height;
            in >> ob.color[0] >> ob.color[1] >> ob.color[2];
            in >> ob.ambient_component >> ob.diffuse_component >> ob.specular_component >> ob.reflection_component;
            in >> ob.specular_exponent;

            pyramid_arr.push_back(ob);
        }
    }

    point ob;
    in >> number_of_light;

    for (int i = 0; i < number_of_light; ++i)
        in >> ob.x >> ob.y >> ob.z, lights.push_back(ob);

    in.close();
}

void drawCheckerBoard(int length)
{
    int black = 0;

    for (int i = -1000; i <= 1000; i += length)
    {
        for (int j = -1000; j <= 1000; j += length)
        {
            if (black == 1)
                glColor3f(1, 1, 1);
            else
                glColor3f(0, 0, 0);

            glBegin(GL_QUADS);
            {
                glVertex3f(i, j, 0);
                glVertex3f(i, j + length, 0);
                glVertex3f(i + length, j + length, 0);
                glVertex3f(i + length, j, 0);
            }
            glEnd();

            black ^= 1;
        }
    }
}

void drawObject()
{
    for (int i = 0; i < number_of_sphere; ++i)
        sphere_arr[i].draw();

    for (int i = 0; i < number_of_pyramid; ++i)
        pyramid_arr[i].generateTriangle(), pyramid_arr[i].draw();
}

void capture()
{
    bitmap_image img(pixel_size, pixel_size);

    double plane_distance = (window_height / 2.0) / tan((pi * fovY) / 360.0);

    point topleft, corner, L = l, U = u, R = r;

    cout << L.x << " " << L.y << " " << L.z << endl;

    L.scalar_multiplication(plane_distance);
    R.scalar_multiplication(window_width / 2.0);
    U.scalar_multiplication(window_height / 2.0);

    topleft = sub(add(add(p, L), U), R);

    double du = (1.0 * window_width) / pixel_size;
    double dv = (1.0 * window_height) / pixel_size;

    int idx;
    double t, mn;

    for (int i = 0; i < pixel_size; ++i)
    {
        for (int j = 0; j < pixel_size; ++j)
        {
            corner.x = topleft.x + (i * du * r.x) - (j * dv * u.x);
            corner.y = topleft.y + (i * du * r.y) - (j * dv * u.y);
            corner.z = topleft.z + (i * du * r.z) - (j * dv * u.z);

            ray tr(p, sub(corner, p));

            idx = -1, mn = 1e9 * 1.0;

            for (int k = 0; k < number_of_sphere; ++k)
            {
                t = sphere_arr[k].intersect_point(tr);

                if (t > 0 && t < mn)
                    mn = t, idx = k;
            }

            if (idx != -1)
            {
                t = sphere_arr[idx].intersect_point(tr);
                img.set_pixel(i, j, 255 * 1, 255 * 1, 255 * 1);
            }
        }
    }

    img.save_image("1505100.bmp");
    img.clear();
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '0':
            capture();
            break;

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

    gluLookAt(p.x, p.y, p.z, p.x + l.x, p.y + l.y, p.z + l.z, u.x, u.y, u.z);

    glMatrixMode(GL_MODELVIEW);

    drawCheckerBoard(30);
    drawObject();

    glutSwapBuffers();
}

void animate()
{
    angle += 1;
    glutPostRedisplay();
}

void init()
{
    drawgrid = 1;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;
    fovY = 80;

    glClearColor(0,0,0,0);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(fovY, 1, 1, 1000.0);
}

int main(int argc, char **argv)
{
    read_input();

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

