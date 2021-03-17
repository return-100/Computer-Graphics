#include <bits/stdc++.h>

using namespace std;

const double pi = acos(-1);
const double del = 0.000000000001;

string command;
double angle, fovY, near, far, aspectRatio;

class point
{
public:

    double component[3];

    point()
    {
        component[0] = 0.0;
        component[1] = 0.0;
        component[2] = 0.0;
    }
};

point a, eye, lookAt, up, col[3], unit[3];

point normalize(point a)
{
    double val = 0.0;

    for (int i = 0; i < 3; ++i)
        val += a.component[i] * a.component[i];

    val = sqrt(val);

    for (int i = 0; i < 3; ++i)
        a.component[i] /= val;

    return a;
}

double dot_product(point a, point b)
{
    double ret = 0.0;

    for (int i = 0; i < 3; ++i)
        ret += a.component[i] * b.component[i];

    return ret;
}

point cross_product(point a, point b)
{
    point ret;

    ret.component[0] = a.component[1] * b.component[2] - a.component[2] * b.component[1];
    ret.component[1] = a.component[2] * b.component[0] - a.component[0] * b.component[2];
    ret.component[2] = a.component[0] * b.component[1] - a.component[1] * b.component[0];

    return ret;
}

point rodrigues_formula(point x, point a, double angle)
{
    point ret, temp_a, temp_b, temp_c = cross_product(a, x);

    for (int i = 0; i < 3; ++i)
        temp_a.component[i] = x.component[i] * cos(angle);

    for (int i = 0; i < 3; ++i)
        temp_b.component[i] = (1.0 - cos(angle)) * dot_product(x, a) * a.component[i];

    for (int i = 0; i < 3; ++i)
        temp_c.component[i] *= sin(angle);

    for (int i = 0; i < 3; ++i)
        ret.component[i] = temp_a.component[i] + temp_b.component[i] + temp_c.component[i];

    return ret;
}

class matrix
{
public:

    int row, col;
    double arr[4][4];

    matrix()
    {

    }

    matrix(int r, int c)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                arr[i][j] = 0.0;
        }

        row = r, col = c;
    }
};

matrix identity(4, 4);

stack <matrix> st;

matrix multiply(matrix a, matrix b)
{
    matrix ret(a.row, b.col);

    for (int i = 0; i < ret.row; ++i)
    {
        for (int j = 0; j < ret.col; ++j)
        {
            for (int k = 0; k < ret.row; ++k)
                ret.arr[i][j] += a.arr[i][k] * b.arr[k][j];
        }
    }

    return ret;
}

matrix w_scaling(matrix a)
{
    if (a.arr[3][0] > del)
    {
        for (int i = 0; i < 4; ++i)
            a.arr[i][0] /= a.arr[3][0];
    }

    return a;
}

void stage1()
{
    ifstream in("scene.txt");
    FILE *stage1 = fopen("stage1.txt", "w");

    for (int i = 0; i < 3; ++i)
        in >> eye.component[i];

    for (int i = 0; i < 3; ++i)
        in >> lookAt.component[i];

    for (int i = 0; i < 3; ++i)
        in >> up.component[i];

    in >> fovY >> aspectRatio >> near >> far;

    while (true)
    {
        in >> command;

        if (command == "triangle")
        {
            for (int k = 0; k < 3; ++k)
            {
                matrix ob(4, 1);
                ob.arr[3][0] = 1;

                for (int i = 0; i < 3; ++i)
                    in >> ob.arr[i][0];

                matrix ans = w_scaling(multiply(st.top(), ob));

                for (int i = 0; i < ans.row - 1; ++i)
                {
                    for (int j = 0; j < ans.col; ++j)
                        fprintf(stage1, "%.7f ", ans.arr[i][j]);
                }

                fprintf(stage1, "\n");
            }

            fprintf(stage1, "\n");
        }
        else if (command == "translate")
        {
            matrix ob = identity;

            for (int i = 0; i < 3; ++i)
                in >> ob.arr[i][3];

            matrix ans = multiply(st.top(), ob);
            st.pop(), st.push(ans);
        }
        else  if (command == "scale")
        {
            matrix ob = identity;

            for (int i = 0; i < 3; ++i)
                in >> ob.arr[i][i];

            matrix ans = multiply(st.top(), ob);
            st.pop(), st.push(ans);
        }
        else if (command == "rotate")
        {
            in >> angle;

            for (int i = 0; i < 3; ++i)
                in >> a.component[i];

            a = normalize(a);

            for (int i = 0; i < 3; ++i)
            {
                unit[i].component[i] = 1.0;
                col[i] = rodrigues_formula(unit[i], a, (angle * pi) / 180.0);
            }

            matrix ob = identity;

            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                    ob.arr[j][i] = col[i].component[j];
            }

            matrix ans = multiply(st.top(), ob);
            st.pop(), st.push(ans);
        }
        else if (command == "push")
            st.push(st.top());
        else if (command == "pop")
            st.pop();
        else
            break;
    }

    in.close();
    fclose(stage1);
}

void stage2()
{
    ifstream in("stage1.txt");
    FILE *stage2 = fopen("stage2.txt", "w");

    point l, r, u;

    for (int i = 0; i < 3; ++i)
        l.component[i] = lookAt.component[i] - eye.component[i];

    l = normalize(l);
    r = normalize(cross_product(l, up));
    u = cross_product(r, l);

    matrix T = identity, R = identity;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i == 0)
                R.arr[i][j] = r.component[j];
            else if (i == 1)
                R.arr[i][j] = u.component[j];
            else
                R.arr[i][j] = -l.component[j];
        }
    }

    for (int i = 0; i < 3; ++i)
        T.arr[i][3] = -eye.component[i];

    matrix V = multiply(R, T);

    while (!in.eof())
    {
        for (int k = 0; k < 3; ++k)
        {
            matrix ob(4, 1);
            ob.arr[3][0] = 1;

            for (int i = 0; i < 3; ++i)
                in >> ob.arr[i][0];

            if (in.eof())
                break;

            matrix ans = w_scaling(multiply(V, ob));

            for (int i = 0; i < ans.row - 1; ++i)
            {
                for (int j = 0; j < ans.col; ++j)
                    fprintf(stage2, "%.7f ", ans.arr[i][j]);
            }

            fprintf(stage2, "\n");
        }

        if (in.eof())
            break;

        fprintf(stage2, "\n");
    }

    in.close();
    fclose(stage2);
}

void stage3()
{
    ifstream in("stage2.txt");
    FILE *stage3 = fopen("stage3.txt", "w");

    double fovX = fovY * aspectRatio;
    double t = near * tan((fovY / 2.0) * (pi / 180.0));
    double r = near * tan((fovX / 2.0) * (pi / 180.0));

    matrix P(4, 4);

    P.arr[0][0] = near / r;
    P.arr[1][1] = near / t;
    P.arr[2][2] = -(far + near) / (far - near);
    P.arr[2][3] = -(2 * far * near) / (far - near);
    P.arr[3][2] = -1;

    while (!in.eof())
    {
        for (int k = 0; k < 3; ++k)
        {
            matrix ob(4, 1);
            ob.arr[3][0] = 1;

            for (int i = 0; i < 3; ++i)
                in >> ob.arr[i][0];

            if (in.eof())
                break;

            matrix ans = w_scaling(multiply(P, ob));

            for (int i = 0; i < ans.row - 1; ++i)
            {
                for (int j = 0; j < ans.col; ++j)
                    fprintf(stage3, "%.7f ", ans.arr[i][j]);
            }

            fprintf(stage3, "\n");
        }

        if (in.eof())
            break;

        fprintf(stage3, "\n");
    }

    in.close();
    fclose(stage3);
}

void init()
{
    for (int i = 0; i < 4; ++i)
        identity.arr[i][i] = 1;

    st.push(identity);
}

int main()
{
    init();
    stage1();
    stage2();
    stage3();

    return 0;
}
