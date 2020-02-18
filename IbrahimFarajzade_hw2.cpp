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
#include<time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  10 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct{
	float x, y;
}point_t;

typedef struct{
	float r, g, b;
}color_t;

typedef struct{
	point_t pos;
	color_t color;
	bool hit;
	bool ready;
	int score;
}lamp_t;

lamp_t lamps[5];
int lamps_ready = 0;
point_t fire = {-326,10};
bool fired = false;
int score = 0;
//time is 20.000 because 20 second is equal to 20.000 milisecond, timer runs with miliseconds.
int time_ = 20000;
bool gameOver = false;


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

void displayTimeScore(){
	vprint(-380,-290, GLUT_BITMAP_9_BY_15, "Time: %d, Score: %d", time_ / 1000, score);
}

void displayLamp(lamp_t lamp){

	//score
	glColor3ub(0, 0, 0);
	//we add positions of x and y to the object
	vprint(lamp.pos.x - 6 , lamp.pos.y + 53, GLUT_BITMAP_TIMES_ROMAN_24, "%d", lamp.score);

	glColor3ub(50, 50, 50);
	circle(lamp.pos.x + 0, lamp.pos.y + 42, 8);

	glColor3ub(255, 255, 255);
	circle(lamp.pos.x + 0, lamp.pos.y + 48, 2);

	glColor3ub(100,100,100);
	glRectf(lamp.pos.x + -10, lamp.pos.y + 19, lamp.pos.x + 10, lamp.pos.y + 42);

	glColor3ub(50, 50, 50);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2f(lamp.pos.x + 10, lamp.pos.y + 40);
	glVertex2f(lamp.pos.x + -10, lamp.pos.y + 36);

	glVertex2f(lamp.pos.x + 10, lamp.pos.y + 36);
	glVertex2f(lamp.pos.x + -10, lamp.pos.y + 32);

	glVertex2f(lamp.pos.x + 10, lamp.pos.y + 32);
	glVertex2f(lamp.pos.x + -10, lamp.pos.y + 28);
	glEnd();

	//main light
	glColor3ub(lamp.color.r, lamp.color.g, lamp.color.b);
	circle(lamp.pos.x + 0, lamp.pos.y + 0, 25);

	//source light, white.
	glColor3ub(255,255,255);
	circle(lamp.pos.x + 0, lamp.pos.y + 0, 8);

	//shining light
	glColor3ub(lamp.color.r * 1.3, lamp.color.g * 1.3, lamp.color.b * 1.3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(lamp.pos.x + 0, lamp.pos.y + 0);
	glVertex2f(lamp.pos.x + -20, lamp.pos.y + -20);

	glVertex2f(lamp.pos.x + 0, lamp.pos.y + 0);
	glVertex2f(lamp.pos.x + -6, lamp.pos.y + -28);

	glVertex2f(lamp.pos.x + 0, lamp.pos.y + 0);
	glVertex2f(lamp.pos.x + 6, lamp.pos.y + -28);

	glVertex2f(lamp.pos.x + 0, lamp.pos.y + 0);
	glVertex2f(lamp.pos.x + 20, lamp.pos.y + -20);
	glEnd();
	glLineWidth(1);


}

void displayGun(){

	glColor3ub(0,0,0);
	glRectf(-400, fire.y + 0, -328, fire.y + 20);

	glColor3ub(255, 255, 255);
	circle(-326, fire.y + 10, 10);

	glColor3ub(0, 0, 0);
	circle(fire.x, fire.y + 10, 5);
	
}

void displayGameOver(){
	glColor3ub(255, 0, 0);
	glLineWidth(3);
	vprint2(-250, 0, 0.5, "-Game Over-");
	glLineWidth(2);
	vprint2(-175, -30, 0.15, "Press F1 to Restart the Game");
}
//
// To display onto window using OpenGL commands
//
void display()
{

	glClearColor(0.9,1,0.9, 0);
	glClear(GL_COLOR_BUFFER_BIT);


	for (int i = 0; i < 5; i++){
		displayLamp(lamps[i]);
	}
	
	displayGun();
	displayTimeScore();
	if (gameOver) displayGameOver();

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
	if (key == ' ') fired = true;

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
	case GLUT_KEY_F1: 
		if (gameOver){			gameOver = false;
			score = 0;
			time_ = 20000;
			for (int i = 0; i < 5; i++){
				lamps[i].ready = false;
				lamps_ready = 0;
			}
		}
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
	// Write your codes here.
	

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


// (px, py) : coordinate of a point, p
// (cx, cy) : center point (centroid) of the circle
// R : radius of the circle
// Result: true (point is withing the circle), otherwise false.
bool checkCircle(float px, float py, float cx, float cy, float R) {
	float dx = px - cx;
	float dy = py - cy;
	float d = sqrt(dx*dx + dy*dy);
	return d <= R;
}

// When you move mouse pointer, this event handler is called automatically
void onMove(int x, int y) {
	// Write your codes here.

	// Convert coordinate (x,y) from GLUT to OpenGL
	int mx = x - winWidth / 2;
	int my = winHeight / 2 - y;

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

	if (time_ <= 0) gameOver = true;
	if (up) fire.y+=2;
	if (down) fire.y-=2;

	if (!gameOver){

		if (lamps_ready < 5){
			printf("ready <5\n");
			for (int i = 0; i < 5; i++){
				if (!lamps[i].ready){
					printf("lamp is preparing..\n");
					lamps[i].color.r = rand() % 256;
					lamps[i].color.g = rand() % 256;
					lamps[i].color.b = rand() % 256;

					//random x between -300 and 350
					lamps[i].pos.x = rand() % 650 - 300;
					//random y between 250 and 350
					lamps[i].pos.y = rand() % 100 + 250;
					lamps[i].ready = true;
					lamps[i].hit = false;
					lamps[i].score = rand() % 5 + 1;
					lamps_ready++;
				}
			}
		}

		for (int i = 0; i < 5; i++){
			if(lamps[i].ready) lamps[i].pos.y -= 0.8;
			if (lamps[i].pos.y < -270){
				lamps[i].ready = false;
				lamps_ready--;
			}

			if (checkCircle(fire.x + 10, fire.y, lamps[i].pos.x, lamps[i].pos.y, 25)){
				lamps[i].hit = true;
				lamps[i].ready = false;
				score += lamps[i].score;
				lamps_ready--;
			}
		}

		if (fired){
			fire.x += 5;
			if (fire.x >= 400){
				fired = false;
				fire.x = -326;
			}
		}

		//time_ decreases by 10.
		time_ -= TIMER_PERIOD;
	}//end of if not gameover

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));

	
}


void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("HW2 - Ibrahim Farajzade");

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