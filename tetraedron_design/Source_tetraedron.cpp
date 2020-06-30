#include <cstdio>
#include <cmath>
#include <GL\glut.h> 

/* 
  Please Download the follow package in order to run the code:https://www.nuget.org/packages/nupengl.core/0.1.0.1?_src=template
  CODE MODIFIED BY NASTOS VASILEIOS ON 30/06/2020
  DEPARTMENT OF INFORMATICS AND TELECOMMUNICATIONS
*/


GLfloat eyex, eyey, eyez;
GLfloat tox, toy, toz;
GLfloat viewupx, viewupy, viewupz;
GLsizei width = 640, height = 480;
bool startstate = true;
bool changep = true;
float rotx = 0.0, roty = 0.0;
float tx = 0, ty = 0;
float zoom = 0;
int lastx = 0, lasty = 0;
bool buttonPressed[3];
bool mouseenable = false;

//----------------------------
void read_copyright()
{
	SetConsoleOutputCP(65001);
	char line[200];
	FILE* fp = fopen("copyrights.txt", "r");
	while (fgets(line, 200, fp) != NULL)
	{
		printf("%s", line);
	}
	fclose(fp);
}
//----------------------------

void drawAxes()
{
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(100, 0, 0);
	glEnd();
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x00AA);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(-100, 0, 0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);


	//y axis

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 100, 0);
	glEnd();
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x00AA);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -100, 0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	//z axis
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 100);
	glEnd();
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x00AA);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -100);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

void Triangle(float a)
{
	const GLfloat n[4][3] =
	{
	{sqrt(6) * a * a / 6,sqrt(2) * a * a / 2,sqrt(3) * a * a / 6},
	{-(sqrt(6) * a * a) / 3,0.0,sqrt(3) * a * a / 6},
	{sqrt(6) * a * a / 6,-(sqrt(2) * a * a) / 2,sqrt(3) * a * a / 6},
	{ 0.0,0.0,-1.0} };
	const GLint faces[4][3] =
	{
	{ 0,2,3},
	{ 0,1,3 },
	{ 1,2,3 },
	{ 0,1,3 } };
	const GLfloat v[4][3] =
	{
	{ -a * sqrt(3) / 6,a / 2,0 },
	{ -a * sqrt(3) / 6,-a / 2,0.0},
	{ a * sqrt(3) / 3,0.0,0.0},
	{ 0.0,0.0,a * sqrt(6) / 3} };

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

void cbMouseMove(int x, int y)
{
	const int dx = x - lastx;
	const int dy = y - lasty;
	lastx = x;
	lasty = y;

	if (buttonPressed[0])
	{
		rotx += (float)1.5 * dy;
		roty += (float)1.5 * dx;
	}
	else if (buttonPressed[1])
	{
		zoom -= (float)2.0 * dy;
	}
	else if (buttonPressed[2])
	{
		tx += (float)0.7 * dx;
		ty -= (float)0.7 * dy;
	}

	glutPostRedisplay();
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


	const GLfloat mat_ambient[] = { 0.0, 0.3, 0.3, 1.0 };
	const GLfloat mat_diffuse[] = { 0.0, 0.5, 0.6, 1.0 };
	const GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	const GLfloat mat_shininess[] = { 32.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// Other configuration options

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glShadeModel(GL_SMOOTH);

}


void display()
{
	float g = 60.0;
	drawAxes();
	if (startstate)
	{
		//Arxiko tetraedro
		glPushMatrix();
		glColor3f(1, 1, 0);
		Triangle(60.0);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(0, 0, g * sqrt(6) / 6);
		Triangle(g);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(-g * sqrt(3) / 12, g / 4, 0);
		Triangle(g);
		glPopMatrix();

		glPushMatrix();
		glColor3f(1.0, 0.0, 1.0);
		glTranslatef(-g * sqrt(3) / 12, -g / 4, 0);
		Triangle(g);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0, 1, 0);
		glTranslatef(g * sqrt(3) / 6, 0, 0);
		Triangle(g);
		glPopMatrix();

		glTranslatef(0.0, 0.0, -zoom);
		glTranslatef(tx, ty, 0.0);
		glRotatef(rotx, 1.0, 0.0, 0.0);
		glRotatef(roty, 0.0, 1.0, 0.0);
	}
	GLfloat light_position[] = { 2.0, 3.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 't' || key == 'T')
	{
		startstate = !startstate;
		glutPostRedisplay();
	}
	if (key == 'e' || key == 'E')
	{
		glutDestroyWindow(glutGetWindow());
		exit(0);
	}
	if (key == 'p' || key == 'P')
	{
		//Prooptiki provoli
		changep = false;
		glutPostRedisplay();
	}
	if (key == 'O' || key == 'o')
	{
		changep = true;
		glutPostRedisplay();
	}
}

void reshape(int newWidth, int newHeight)
{

	printf("Reshape event detected.\n");

	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(width, height);

	glutCreateWindow("TETRAEDRO");

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glMatrixMode(GL_PROJECTION);

	//epilogi provolis
	if (changep)
		glOrtho(-150, 150, -150, 150, -150, 150);
	else
		gluPerspective(80, (float)width / (float)height, 1, 70);

	eyex = 2;
	eyey = 5;
	eyez = 1;
	tox = 0;
	toy = 0;
	toz = 0;
	viewupx = 0;
	viewupy = 1;
	viewupz = 0;
	gluLookAt(eyex, eyey, eyez, tox, toy, toz, viewupx, viewupy, viewupz);

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(cbMouseClick);
	glutMotionFunc(cbMouseMove);


	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	graphicsInit();
	glutMainLoop();

	return 0;
}
