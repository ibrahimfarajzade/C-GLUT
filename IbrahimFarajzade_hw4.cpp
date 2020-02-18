/*
Name: Ibgrahim Farajzade
Section: 1
ID: 21503494
*/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"
#include <time.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  10 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer
#define PI 3.1415

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
bool lighting = true;
int  winWidth, winHeight; // current Window width and height
float earthA = 180;
int randomDir;
float speed;



typedef struct{
	float x, y;
}point_t;
typedef struct{
	float r, g, b;
}color_t;
typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
} light_t;
typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;
color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}
color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}
// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}
color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) *distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

vertex_t mVertex;

light_t  light = { { 0, 0 }, {1,1,1}, {0,1} };
float earthTrans = 250;
point_t trans;

float distance(point_t p, point_t q) {
	return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

void vertex(point_t P, point_t Tr, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);
}

// this is function signature (prototype)
// this is called "function declaration"
void onTimer(int v);

//
// to draw circle, center at (x,y)
//  radius r
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


void circle_lighting(int x, int y, int r){
	float angle;
	color_t color;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		mVertex.pos.x = x + r*cos(angle);
		mVertex.pos.y = y + r*sin(angle);
		color = calculateColor(light, mVertex);
		glColor3f(color.r, color.g, color.b);
		vertex({ mVertex.pos.x, mVertex.pos.y }, { trans.x, trans.y}, ((earthA-90)*D2R) );
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


void displayBg(){
	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex2f(-300, 300);
	glVertex2f(300, 300);
	glColor3f(0.5, 0.5, 0.5);
	glVertex2f(300, -300);
	glVertex2f(-300, -300);
	glEnd();


}

void displaySun(){
	if (!lighting)
		glColor3f(0, 0, 0);
	else
		glColor3f(1, 1, 1);

	circle(0, 0, 80);
}
void displayEarth(){
	if (lighting)
		circle_lighting(0, 0, 20);
	else
		circle(trans.x, trans.y, 20);
}

void displayNameInfo(){
	glColor3f(0.9, 0.9, 0.9);
	vprint(-290, 280, GLUT_BITMAP_9_BY_15, "Ibrahim Farajzade - hw:4");

	if (!lighting)
		vprint(-190, -290, GLUT_BITMAP_9_BY_15, "Press space bar to turn ON the sun's light");
	else
		vprint(-190, -290, GLUT_BITMAP_9_BY_15, "Press space bar to turn OFF the sun's light");
}
//
// To display onto window using OpenGL commands
//
void display()
{

	glClearColor(0.9, 1, 0.9, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBg();
	displaySun();
	displayEarth();
	displayNameInfo();

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

	if (key == ' '){
		lighting = !lighting;
	}


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
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}



// When you move mouse pointer, this event handler is called automatically
void onMove(int x, int y) {
	// Write your codes here.

	// Convert coordinate (x,y) from GLUT to OpenGL
	float mx = x - winWidth / 2;
	float my = winHeight / 2 - y;

	// modify the flag that shows if the mouse is on the button.
	// start button circle = (0, 0 ) with radius 70
	//checkCircle(mx, my, 0, 0, 70);


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	earthA+= randomDir * speed;

	trans.x = cos(earthA * D2R) * earthTrans;
	trans.y = sin(earthA * D2R) * earthTrans;

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));
	mVertex.N = { 0, 1 };
	speed = (rand() % 5 +1) / 10.0;
	randomDir = rand() % 2 ? randomDir = 1 : randomDir = -1;

}


void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("HW4 - Ibrahim Farajzade");

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
