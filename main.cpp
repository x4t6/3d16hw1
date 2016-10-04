//3D game programming 2016
//lab2: 2D


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
//#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "RGBpixmap.h"

// Global variables for measuring time (in milli-seconds)
int startTime;
int prevTime;

//  http://devmaster.net/forums/topic/7934-aabb-collision/
bool AABBtest(float ax1, float ay1, float ax2, float ay2, float bx1, float by1, float bx2, float by2)
{
    return
        ax1 > bx2 || ax2 < bx1 ||
        ay1 > by2 || ay2 < by1;
}

static void CheckError(int line)
{
   GLenum err = glGetError();
   if (err) {
      printf("GL Error %s (0x%x) at line %d\n",
             gluErrorString(err), (int) err, line);
   }
}

//Set windows
int screenWidth = 800 , screenHeight = 600;

int i=0;
RGBApixmap pic[3]; // create two (empty) global pixmaps
RGBApixmap bg;
int whichPic = 0; // which pixmap to display
int picX=100, picY=100;

int rectX=300, rectY=100;

float rotation_test=0;
float scale_test=0.3;


int jumpState=0;
int DirectState=0;  //0:right  1:left
int Gamescore=0;

void init();

//<<<<<<<<<<<<<<<<<<<<<<<<< myMouse >>>>>>>>>>>>>>>>>>>>>>>>
/*void myMouse(int button, int state, int mx, int my)
{ // set raster position with a left click
	if(button == GLUT_LEFT_BUTTON )
	{


		glutPostRedisplay();
	}

}*/
//<<<<<<<<<<<<<<<<<<<<<<<<< mouseMove >>>>>>>>>>>>>>>>>
/*void mouseMove(int x, int y)
{// set raster position with mouse motion
	//rasterPos.x = x; rasterPos.y = screenHeight - y;
	//glRasterPos2i(rasterPos.x, rasterPos.y);
	glutPostRedisplay();
}*/

//myReshape
void myReshape(int w, int h)
{
    /* Save the new width and height */
    screenWidth  = w;
    screenHeight = h;

    /* Reset the viewport... */
    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, (GLfloat)screenWidth, 0.0, (GLfloat)screenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//myDisplay
void myDisplay(void)
{
	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;
	prevTime = currTime;

	char fpsmss[30];
	sprintf(fpsmss, "Fps %.1f", 1000.0/timeSincePrevFrame);


	glClear(GL_COLOR_BUFFER_BIT);

    //draw background
    glRasterPos2i(50, 50);
    bg.blend();




	//change direction
	if(DirectState==0) {
		pic[whichPic].blendTex(picX, picY);
		
	}else {
		int offset = pic[whichPic].nCols;
		pic[whichPic].blendTex(picX+offset, picY, -1, 1);
	}

	//rotation test
	pic[whichPic].blendTexRotate(100, 250, 1, 1, rotation_test);

	//scale test
	pic[whichPic].blendTexRotate(250, 250, scale_test, scale_test);

	//bouding box outside test
    bool Hit = !AABBtest(picX, picY, picX + pic[whichPic].w(), picY + pic[whichPic].h(), rectX, rectY, rectX + 25, rectY + 25);
	if(Hit) {
        glColor3f(1.0f, 0.0f, 0.0f);
		Gamescore -= 1;
	}else {
	    glColor3f(0.0f, 1.0f, 0.0f);
	}
	glRectf(rectX, rectY, rectX + 25, rectY + 25);


	//Font
	char mss[30];
	sprintf(mss, "Score %d", Gamescore);

	glColor3f(1.0, 0.0, 0.0);  //set font color
    glRasterPos2i(10, 550);    //set font start position
	for(int i=0; i<strlen(mss); i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, mss[i]);
	}

	glColor3f(0.0, 0.5, 0.3);  //set font color
    glRasterPos2i(screenWidth-100, 550);    //set font start position
	for(int i=0; i<strlen(fpsmss); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, fpsmss[i]);
	}

	CheckError(__LINE__);

	glutSwapBuffers();
}



void SpecialKeys(int key, int x, int y)
{
	// this string keeps the last good setting for the game mode
    //char gameModeString[40] = "800x600";

	switch(key) {
	/*	case GLUT_KEY_UP:
			picY += 5;
			break;
		case GLUT_KEY_DOWN:
			picY -= 5;
			break;*/
		case GLUT_KEY_LEFT:
			picX -= 5;
			if (whichPic==0) whichPic=1;
			else whichPic=0;
			DirectState=1;
			break;
		case GLUT_KEY_RIGHT:
			picX += 5;
			if (whichPic==0) whichPic=1;
			else whichPic=0;
			DirectState=0;
			break;

	    /*
		case GLUT_KEY_F1:
			// define resolution, color depth
			glutGameModeString("640x480:32");
			// enter full screen
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
				glutEnterGameMode();
				sprintf(gameModeString,"640x480:32");
				// register callbacks again
				// and init OpenGL context
				init();
			}
			else
				glutGameModeString(gameModeString);
			break;

	    case GLUT_KEY_F2:
				glutFullScreen();
			break;
		case GLUT_KEY_F6:
			// return to default window
			if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) != 0)
				glutLeaveGameMode();
			break;
		*/
		

	}
	glutPostRedisplay();
}



void jump(int i)
{
    whichPic=2;

	if(i<10) {
		if (i<5) {
			picY+=15;
		} else {
			picY-=15;
		}
		i++;
		glutTimerFunc( 100, jump, i);
	}else {
		whichPic=0;
		jumpState=0;
	}

	glutPostRedisplay();
}

//back and forth
void bf(int i)
{

    /*
        Your Implementation
	*/

	glutPostRedisplay();
}


void fly(int i)
{

	/*
        Your Implementation
	*/

	glutPostRedisplay();
}

void update(int i)
{
	double r = double(i)/50.0;
	
	rectX = 400 + 250*sin(r);

	rotation_test += 2;
	scale_test += 0.01;

	++i;
	glutTimerFunc( 33, update, i);
	glutPostRedisplay();
}


//<<<<<<<<<<<<<<<<<<<<<<<< myKeys >>>>>>>>>>>>>>>>>>>>>>
void myKeys(unsigned char key, int x, int y)
{
	switch(key)
	{
        case 'Q':
        case 'q':
			exit(0);
			break;

		case 'm':
		case ' ':
			if(jumpState==0) {
				jumpState=1;
				Gamescore++;
				jump(0);
			}
			break;

        case 'b':
				bf(0);
			break;

        case 'f':
				fly(0);
			break;
	} //switch(key)

	glutPostRedisplay();
}


void init()
{
	//GLenum err = glewInit();
	//if (GLEW_OK != err)
	//{
	//  // Problem: glewInit failed, something is seriously wrong.
	//  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	//}
	
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(myKeys);
//	glutMouseFunc(myMouse);
//	glutMotionFunc(mouseMove);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);

	glShadeModel(GL_SMOOTH); 
	//glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //background color(1.0, 1.0, 1.0): white color
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(50, 30);
	glutCreateWindow("2D World");

	init();

	srand(time(0));  //rand seed

	cout<<"Reading sprite";
    pic[0].readBMPFile("image/stand.bmp");  cout<<'.';
	pic[1].readBMPFile("image/walk.bmp");  cout<<'.';
    pic[2].readBMPFile("image/fly.bmp");  cout<<'.'<<endl;

	for (int i=0; i<3; i++) pic[i].setChromaKey(232, 248, 248);

	//cout<<"Reading Backgroud........"<<endl;
	//bg.readBMPFile("image/dog.bmp");

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	update(0);
	glutMainLoop();

	return 0;
}

