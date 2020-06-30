#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <GL/glut.h>

/* 
  Please Download the follow package in order to run the code:https://www.nuget.org/packages/nupengl.core/0.1.0.1?_src=template
  CODE MODIFIED BY NASTOS VASILEIOS ON 30/06/2020
  DEPARTMENT OF INFORMATICS AND TELECOMMUNICATIONS
*/

using namespace std;

int lastx = 0, lasty = 0;    // Last pointer position
float rotx = 10.0, roty = -30.0; // Rotations about x and y axes
float tx = 0.0, ty = 0.0;     // translate along x and y axes
float zoom = 1.0;                // translate along z axis
bool buttonPressed[3];           // button state
bool fogOn = true;               // default is to use fog                   
GLfloat mat_ambient[4];


const int MaxDepth = 5;          // Maximum level of recursion
int recursionDepth = 1;	         // Level of recursion for Menger sponge
GLfloat light_ambient[4];
GLfloat r, g, b;

void obtain_copyright()
{
    SetConsoleOutputCP(65001);
    string line;
    ifstream cop;
    cop.open("copyrights.txt", std::ios::in);
    while (getline(cop, line))
    {
        cout << line << endl;
    }
    cop.close();
}

void drawSolidTriangle(float a)
{
    const GLfloat n[4][3] =
    {
    {sqrt(6) * a * a / 6,sqrt(2) * a * a / 2,sqrt(3) * a * a / 6},
    {-sqrt(6) * a * a / 3,0,sqrt(3) * a * a / 6},
    {sqrt(6) * a * a / 6,-sqrt(2) * a * a / 2,sqrt(3) * a * a / 6},
    {0,0,-1} };
    const GLint faces[4][3] =
    {
    { 0,3,2},
    { 0,1,3},
    { 1,2,3},
    { 0,2,1} };
    const GLfloat v[4][3] =
    {
    { -sqrt(3) * a / 6,a / 2,0.0},
    { -sqrt(3) * a / 6,-a / 2,0},
    { a * sqrt(3) / 3,0,0},
    { 0,0,a * sqrt(6) / 3} };
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++)
    {
        glNormal3fv(&n[i][0]);
        glVertex3fv(&v[faces[i][0]][0]);
        glVertex3fv(&v[faces[i][1]][0]);
        glVertex3fv(&v[faces[i][2]][0]);
    }
    glEnd();
    glPopMatrix();
}

void drawTriangle(float len, float depth)
{
    if (depth == 0)
    {
        drawSolidTriangle(len);
    }
    else
    {
        depth--;
        len /= 2.0;
        glPushMatrix();
        glColor3f(1, 0, 0);
        mat_ambient[0] = 1;
        mat_ambient[1] = 0;
        mat_ambient[2] = 0;
        mat_ambient[3] = 0;
        glTranslatef(0, 0, len * sqrt(6) / 6);
        drawTriangle(len, depth);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 0, 1);
        mat_ambient[0] = 0;
        mat_ambient[1] = 0;
        mat_ambient[2] = 1;
        mat_ambient[3] = 0;
        glTranslatef(-len * sqrt(3) / 12, len / 4, 0.0);
        drawTriangle(len, depth);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glPopMatrix();

        glPushMatrix();
        glColor3f(1, 0, 1);

        glTranslatef(-len * sqrt(3) / 12, -len / 4, 0.0);
        mat_ambient[0] = 1;
        mat_ambient[1] = 0;
        mat_ambient[2] = 1;
        mat_ambient[3] = 0;
        drawTriangle(len, depth);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 0);
        mat_ambient[0] = 0;
        mat_ambient[1] = 1;
        mat_ambient[2] = 0;
        mat_ambient[3] = 0;
        glTranslatef(len * sqrt(3) / 6, 0.0, 0.0);
        drawTriangle(len, depth);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glPopMatrix();
    }
}

void graphicsInit(void)
{
    const GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    const GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    const GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    const GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    const GLfloat mat_shininess[] = { 32.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const GLfloat fog_color[] = { 0.9, 0.9, 0.9, 1.0 };

    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.1);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogfv(GL_FOG_COLOR, fog_color);

    glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
}

void cbReshape(GLsizei width, GLsizei height)
{
    const float fieldOfView = 30.0; /* degrees */
    const float nearnew = 0.5;
    const float farnew = 30.0;
    const float aspect = (float)width / (float)height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, aspect, nearnew, farnew);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------

void cbKeyboard(unsigned char key, int x, int y)
{
    if (key - '0' >= 0 && key - '0' <= MaxDepth)
    {
        recursionDepth = (key - '0');
    }
    else if (key == 'f')
    {
        fogOn = !fogOn;
    }
    else if (key == 'q' || key == 'Q' || key == 27)
    {
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }
    cout << "Recurcion depth of drawed Triangle tetraedron is:" << recursionDepth << endl;
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void cbMouseClick(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        buttonPressed[0] = (state == GLUT_DOWN);
        break;
    case GLUT_MIDDLE_BUTTON:
        buttonPressed[1] = (state == GLUT_DOWN);
        break;
    case GLUT_RIGHT_BUTTON:
        buttonPressed[2] = (state == GLUT_DOWN);
        break;
    }
    lastx = x;
    lasty = y;
}

//----------------------------------------------------------------------------

void cbMouseMove(int x, int y)
{
    const int dx = x - lastx;
    const int dy = y - lasty;
    lastx = x;
    lasty = y;

    if (buttonPressed[0])
    {
        rotx += (float)0.5 * dy;
        roty += (float)0.5 * dx;
    }
    else if (buttonPressed[1])
    {
        zoom -= (float)0.05 * dy;
    }
    else if (buttonPressed[2])
    {
        tx += (float)0.01 * dx;
        ty -= (float)0.01 * dy;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void cbDrawPicture(void)
{
    // set viewing transformation
    glLoadIdentity();

    GLfloat light_position[] = { 2.0, 3.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (fogOn) glEnable(GL_FOG);

    // draw object

    glTranslatef(0.0, 0.0, -zoom);
    glTranslatef(tx, ty, 0.0);
    glRotatef(rotx, 1.0, 0.0, 0.0);
    glRotatef(roty, 0.0, 1.0, 0.0);
    drawTriangle(1.0, recursionDepth);
    glDisable(GL_FOG);

    glFlush();
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    //copyrights for modification
    obtain_copyright();
    // Initialize GLUT and register callback functions

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 50);
    glOrtho(-100, 100, -100, 100, -100, 100);
    glutCreateWindow("Serpinski Tetrahedron");
    gluLookAt(0.0, 0.0, 2.5,   // eye position
        0.0, 0.0, 0.0,   // look-at point
        0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT, GL_FILL);

    glutReshapeFunc(cbReshape);
    glutKeyboardFunc(cbKeyboard);
    glutMouseFunc(cbMouseClick);
    glutMotionFunc(cbMouseMove);
    glutDisplayFunc(cbDrawPicture);

    // Initialize and enter event loop (never returns)

    if (argc > 1)
    {
        recursionDepth = atoi(argv[1]);
        if (recursionDepth < 0) recursionDepth = 0;
        if (recursionDepth > MaxDepth) recursionDepth = MaxDepth;
    }

    graphicsInit();

    cout << endl;
    cout << "\nCommands:\n";
    cout << "\tleft mouse button:    rotate object" << endl;
    cout << "\tmiddle mouse button:  zoom object" << endl;
    cout << "\tright mouse button:   translate object" << endl;
    cout << "\t0 - " << MaxDepth
        << ":                set level of recursion" << endl;
    cout << "\tf:                    toggle fog" << endl;
    cout << "\tq:                    quit" << endl;

    glutMainLoop();

    return 0;
}
