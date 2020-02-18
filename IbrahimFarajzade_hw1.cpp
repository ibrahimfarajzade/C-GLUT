#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  30 // Period for the timer.
#define TIMER_ON      1    // 0:disable timer, 1:enable timer

#define D2R 0.0174532

bool up = false, down = false, right = false, left = false;
bool space = false;

float mx, my;

typedef struct{
	float x, y;
	int dir;
}speaker;

speaker speakers[10];
int numClicks = 0;
int last = 0;
int protectedIndex = 0;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}


// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawSpeaker(float x, float y){
	
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x+0, y+0);
	glVertex2f(x - 10, y + 25);
	glVertex2f(x - 10, y + 125);
	glVertex2f(x + 0, y + 150);
	glVertex2f(x + 60, y + 150);
	glVertex2f(x + 70, y + 125);
	glVertex2f(x + 70, y + 25);
	glVertex2f(x + 60, y + 0);
	glEnd();

	glColor3f(0.6, 0.4, 0.4);
	glRectf(x + 0, y + 120, x + 60, y + 140);

	glColor3f(0, 0, 0);
	circle(x + 10, y + 130, 8);

	glColor3f(0.6, 0.4, 0.4);
	circle(x+18, y + 130, 9);

	glColor3f(0,0,0);
	vprint(x + 18, y + 127, GLUT_BITMAP_TIMES_ROMAN_10, "Logitech");



	glColor3f(0.8, 1, 0.8);
	circle(x + 30, y + 35, 20);

	glColor3f(0, 0, 0);
	circle(x + 30, y + 35, 17);

	glColor3f(1,1,1);
	circle(x + 30, y + 35, 5);

	glColor3f(0.8, 1, 0.8);
	circle(x + 30, y + 90, 20);

	glColor3f(0, 0, 0);
	circle(x + 30, y + 90, 17);

	glColor3f(1, 1, 1);
	circle(x + 30, y + 90, 5);



	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	glVertex2f(x + 5, y-10);
	glVertex2f(x + 55, y-10);
	glVertex2f(x + 65, y-40);
	glVertex2f(x-5, y-40);
	glEnd();

	glColor3f(0.9, 0.2, 0.2);
	glBegin(GL_QUADS);
	glVertex2f(x + 20, y + 0);
	glVertex2f(x + 40, y + 0);
	glVertex2f(x + 45, y-30);
	glVertex2f(x + 15, y-30);
	glEnd();
}

//
// To display onto window using OpenGL commands
//

int protectIndex(int index){
	return index > 10 ? 10 : index;
}


void display()
{

	//
	// clear window to black
	//
	glClearColor(0.6, 0.6, 0.6, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < protectedIndex; i++){
		drawSpeaker(speakers[i].x, speakers[i].y);
	}
	

	glColor3f(1,0,0);
	vprint(300, -270, GLUT_BITMAP_TIMES_ROMAN_10, "(%.0f, %.0f)", mx,my);

	glColor3f(0, 0, 0);
	glRectf(-380,280,-250,240);
	glColor3f(1,1,1);
	vprint(-365, 265, GLUT_BITMAP_HELVETICA_12, "Ibrahim Farajzade");
	vprint(-345, 250, GLUT_BITMAP_HELVETICA_12, "21503494");


	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		space = !space;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	x = x - WINDOW_WIDTH / 2;
	y = WINDOW_HEIGHT / 2 - y;

	// Write your codes here.
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN){
		if (y >= 150)
			y = 149;
		if (y <= -260)
			y = -259;
	
	speakers[last].x = x;
	speakers[last].y = y;
	numClicks++;
	last = numClicks % 10;
	
	protectedIndex = protectIndex(numClicks);

	}


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.
	x = x - WINDOW_WIDTH / 2;
	y = WINDOW_HEIGHT / 2 - y;

	mx = x;
	my = y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.


	for (int i = 0; i < protectedIndex; i++){
		if (speakers[i].y >= 150 || speakers[i].y <= -260)
			speakers[i].dir *= -1;
		
		if (space){
			speakers[i].y += 0.7 * speakers[i].dir;
		}
		
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {
	for (int i = 0; i < 10; i++){
		speakers[i].dir = 1;
	}
}


void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Template File");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}