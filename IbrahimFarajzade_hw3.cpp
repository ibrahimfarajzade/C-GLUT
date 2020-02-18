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

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
#define START 0
#define PLAY 1
#define FIRE 2

#define WRONG 0
#define TRUE 1
#define NOT_SELECTED 2

#define  TIMER_PERIOD  10 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

int gameState = 0;
int questionState = 0;
int direction = 0;

typedef struct{
	float x, y;
}point_t;

typedef struct{
	float r, g, b;
}color_t;

typedef struct{
	int angle;
	color_t c;
	point_t p;
	int score;
	float pos;
}baloon_t;

typedef struct{
	color_t c;
	point_t p;
	int choice;
}c_t;

typedef struct{
	c_t choices[4];
	int num1, num2;
	int answer;
	int state;
}q_t;

point_t fire = { 0, 0 };
bool selected = false;
int selectedIndex = -1;
int score = 0;
int computerScore = 0;
//time is 60.000 because 60 second is equal to 60.000 milisecond, timer runs with miliseconds.
int time_ = 60000;
bool gameOver = false;
q_t q;
point_t mouse;
baloon_t b;
int gunAngle =0;
point_t circ;

float smoothAngle = 0;

color_t yellow = { 1, 1, 0 };
color_t blue = { 0, 0, 1 };
color_t green = { 0, 1, 0 };
color_t red = { 1, 0, 0 };


float distance(point_t p, point_t q) {
	return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y)*(p.y - q.y));
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

void randomQuestion(){
	q.num1 = rand() % 9 + 1;
	q.num2 = rand() % 9 + 1;
	q.answer = q.num1 % q.num2;

	for (int i = 0; i < 4; i++){
		q.choices[i].c = yellow;
		q.choices[i].choice = rand() % 10;
		while (q.choices[i].choice == q.answer)
			q.choices[i].choice = rand() % 10;
		q.choices[i].p = {i*75,280};
		q.state = NOT_SELECTED;
	}

	q.choices[rand() % 4].choice = q.answer;
}

void randomBaloon(){
	b.angle = rand() % 360;
	b.c = { (rand() % 1000) / 1000.0, (rand() % 1000) / 1000.0, (rand() % 1000) / 1000.0 };
	b.score = rand() % 5 + 1;
	b.pos = rand() % 250+50;
}

void displayTimeScore(){
	glColor3f(1, 1, 1);
	if (!gameOver)
		vprint(-180, -285, GLUT_BITMAP_9_BY_15, "Time: %d     Computer: %d      Score: %d", time_ / 1000, computerScore, score);
	else
		vprint(-235, -285, GLUT_BITMAP_9_BY_15, "--Game Over--     Computer: %d      Score: %d", computerScore, score);
}

void displayBaloon(){

	glColor3f(0, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(b.p.x, b.p.y - 22);
	glVertex2f(b.p.x - 5, b.p.y - 27);
	glVertex2f(b.p.x + 5, b.p.y - 27);
	glEnd();
	
	glColor3f(b.c.r, b.c.g, b.c.b);
	circle(b.p.x, b.p.y,25);
	glColor3f(0, 0, 0);
	vprint(b.p.x, b.p.y, GLUT_BITMAP_8_BY_13, "%d", b.score);
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(b.p.x, b.p.y - 75);
	glVertex2f(b.p.x, b.p.y-25);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(b.p.x - 5, b.p.y + 20);
	glVertex2f(b.p.x - 17, b.p.y + 15);
	glVertex2f(b.p.x - 15, b.p.y + 8);
	glVertex2f(b.p.x - 3, b.p.y + 12);
	glEnd();


	

}

void displayGun(){
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-300,0);
	glVertex2f(300, 0);
	glEnd();

	glColor3f(0,0,0);
	circle(0, 0, 27);

	glColor3f(1, 0, 1);
	circle(0, 0, 25);

	glColor3f(1, 1, 0);
	vprint(0, 0, GLUT_BITMAP_HELVETICA_12, "%d", gunAngle);

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(cos(gunAngle* D2R) * 24, sin(gunAngle* D2R) * 24);
	glEnd();


}

void displayBg(){
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex2f(-300, 300);
	glVertex2f(300, 300);
	glColor3f(0.5, 0.5, 0.5);
	glVertex2f(300, -300);
	glVertex2f(-300, -300);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-300, 300);
	glVertex2f(300, 300);
	glVertex2f(300, 260);
	glVertex2f(-300, 260);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-300, -300);
	glVertex2f(300, -300);
	glVertex2f(300, -260);
	glVertex2f(-300, -260);
	glEnd();

}

void displayQuestion(){
	glColor3f(1, 1, 1);
	vprint2(-290, 265,0.3,"%d %% %d = ?", q.num1, q.num2, q.answer);
	for (int i = 0; i < 4; i++){
		glColor3f(q.choices[i].c.r, q.choices[i].c.g, q.choices[i].c.b);
		circle(q.choices[i].p.x, q.choices[i].p.y, 15);
		glColor3f(0, 0, 0);
		vprint(q.choices[i].p.x, q.choices[i].p.y, GLUT_BITMAP_HELVETICA_10, "%d", q.choices[i].choice);
	}

}

void drawCirc(){
	glColor3f(1, 0, 0);
	circle(circ.x, circ.y, 5);
}

void correctAngle(){
	gunAngle = abs(gunAngle);
	gunAngle = gunAngle % 360;
}


int decideDirection(){
	if (abs(gunAngle - b.angle) < 180)
		return -1;
	else
		return 1;
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

color_t decideColor(){
	if (questionState == WRONG)
		return red;
	return green;
}

bool focused(int i){
	return checkCircle(mouse.x, mouse.y, q.choices[i].p.x, q.choices[i].p.y, 15);
}

bool hits(){
	return checkCircle(circ.x, circ.y, b.p.x, b.p.y, 25);
}
//
// To display onto window using OpenGL commands
//
void display()
{

	glClearColor(0.9, 1, 0.9, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBg();
	

	if (gameState == START){
		glColor3f(1, 1, 1);
		vprint(-235, -285, GLUT_BITMAP_9_BY_15, "Press any key to start the game");
		displayGun();
	}
	else if (gameState == PLAY){
		displayQuestion();
		displayTimeScore();
		displayGun();
		displayBaloon();
	}
	else if (gameState == FIRE){
		drawCirc();
		displayQuestion();
		displayTimeScore();
		displayGun();
		displayBaloon();
	}

	glutSwapBuffers();


}

void updateBaloon(){
	b.p = { cos(b.angle * D2R) * b.pos, sin(b.angle * D2R) * b.pos };
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	gameState = PLAY;

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
	// Write your codes here.
	if (!selected){
		for (int i = 0; i < 4; i++){
			if (focused(i)){
				selected = true;
				selectedIndex = i;
				if (q.choices[i].choice == q.answer){
					questionState = TRUE;
					
				}
				else
					questionState = WRONG;
			}
		}
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


void initCirc(){
	circ.x = 0;
	circ.y = 0;
}

void initGame(){
	randomBaloon();
	randomQuestion();
	gameState = PLAY;
	selected = false;
	selectedIndex = -1;
	direction = decideDirection();
	initCirc();
	updateBaloon();
	questionState = NOT_SELECTED;
}

// When you move mouse pointer, this event handler is called automatically
void onMove(int x, int y) {
	// Write your codes here.

	// Convert coordinate (x,y) from GLUT to OpenGL
	float mx = x - winWidth / 2;
	float my = winHeight / 2 - y;
	mouse.x = mx;
	mouse.y = my;

	for (int i = 0; i < 4; i++){
		if (focused(i) && selectedIndex != i){
			q.choices[i].c = blue;
		}
		else if (selectedIndex != i){
			q.choices[i].c = yellow;
		}
	}

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
	smoothAngle++;

	if (gameState != START){

		if (!gameOver){
			time_ -= TIMER_PERIOD;

			if (gameState == PLAY){
				gunAngle++;
				printf("%d\n",gunAngle);
				correctAngle();
				if (gunAngle == b.angle)gameState = FIRE;
				

				//printf("gunAnge: %d, baloon: %d\n", gunAngle, b.angle);

			}

			if (questionState == TRUE){
				b.p.y += 2;
			}

			if (gameState == FIRE){
				circ.x += cos(gunAngle * D2R) * 2;
				circ.y += sin(gunAngle * D2R) * 2;
				if (hits()){
					computerScore += b.score;
					initGame();
				}
				
			}
			
			if (b.p.y > 300){
				score += b.score;
				initGame();
			}
			

		}//end of if not gameover

		if (time_ <= 0) gameOver = true;

		if (selected){
			q.choices[selectedIndex].c = decideColor();
		}

	}
	

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));

	randomQuestion();
	randomBaloon();

	gameState = START;
	randomBaloon();
	randomQuestion();
	selected = false;
	selectedIndex = -1;
	direction = decideDirection();
	initCirc();
	updateBaloon();
	questionState = NOT_SELECTED;

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