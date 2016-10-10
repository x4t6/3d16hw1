//3D game programming 2013
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
        ay1 > by2 || ay2 < by1 ;
}

static void CheckError(int line)
{
    GLenum err = glGetError();
    if (err)
    {
        printf("GL Error %s (0x%x) at line %d\n",
               gluErrorString(err), (int) err, line);
    }
}

//Set windows
int screenWidth = 1280, screenHeight = 720;

//int i=0;
//RGBApixmap pic[3]; // create two (empty) global pixmaps
RGBApixmap bg[2];
RGBApixmap gameStart, gameOver, gameWin;
RGBApixmap fighter ;
RGBApixmap enemy ;
RGBApixmap bullet ;
RGBApixmap enemyBullet ;
RGBApixmap enemy_icon ;
RGBApixmap fighter_icon ;
int bg1X, bg2X, fighter_X, fighter_Y, enemy_X, enemy_Y, bullet_X, bullet_Y, enemyBullet_X, enemyBullet_Y, target_X, target_Y ;
int gameState ;
int enemyRemains, life ;

const int BG_Speed = 1, FI_SPEED = 5, EN_SPEED = 3, BU_SPEED = 10, ENB_SPEED = 12 ;
const int SCOREBOARD_HEIGHT = 120 ;
const int START = 0, OVER = 1, RUN = 2, WIN =3 ;

bool upPressed ;
bool downPressed ;
bool leftPressed ;
bool rightPressed ;
bool fire, enemyIsATK ;
bool enemyHited, fighterHited, fighterCrash ;
/*
int whichPic = 0; // which pixmap to display
int picX=100, picY=100;
int rectX=300, rectY=100;
float rotation_test=0;
float scale_test=0.3;
int jumpState=0;
int DirectState=0;  //0:right  1:left
int Gamescore=0;
*/
void init();

//myReshape
void myReshape(int w, int h)
{
    // Save the new width and height //
    screenWidth  = w;
    screenHeight = h;

    // Reset the viewport... //
    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, (GLfloat)screenWidth, 0.0, (GLfloat)screenHeight, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void enemyATTACK()
{
    if(enemyIsATK==false)
    {
        enemyBullet_X = enemy_X+enemy.w();
        enemyBullet_Y = enemy_Y+enemy.h()/2;
        target_X = fighter_X ;
        target_Y = fighter_Y ;
        enemyIsATK = true ;
    }//if
}//enemyATTACK


//myDisplay
void myDisplay(void)
{
    //glClear(GL_COLOR_BUFFER_BIT);
    switch(gameState)
    {
    case START :
        gameStart.blendTex(0,0) ;
        break ;
    case RUN :
        bg[0].blendTex(bg1X,0) ;
        bg[1].blendTex(bg2X,0);

        fighter.blendTex( fighter_X, fighter_Y ) ;
        enemy.blendTex( enemy_X, enemy_Y ) ;

        for(int i = 0; i<enemyRemains ; i ++)
            enemy_icon.blendTex(30+i*30,600) ;

        for(int i = 0; i<life; i++)
            fighter_icon.blendTex(30+i*50,600+40) ;
//----bg move----//
        bg1X += BG_Speed ;
        bg2X += BG_Speed ;
        if(bg1X>=screenWidth)bg1X = -screenWidth ;
        if(bg2X>=screenWidth)bg2X = -screenWidth ;

//----fighter move----//
        if(upPressed) fighter_Y += FI_SPEED ;
        if(downPressed) fighter_Y -= FI_SPEED ;
        if(leftPressed) fighter_X -= FI_SPEED ;
        if(rightPressed) fighter_X += FI_SPEED ;

//----bullet move----//
        if(fire == true)
        {
            bullet.blendTex(bullet_X,bullet_Y) ;
            bullet_X -= BU_SPEED ;
            if(bullet_X <= 0)
            {
                fire = false ;
                bullet_X = screenWidth ;
                bullet_Y = screenHeight ;
            }//if
        }//if

//----enemy move----//
        enemy_X +=  EN_SPEED ;
        if(enemy_X > screenWidth)
        {
            enemyIsATK = false ;
            enemy_Y = (int)(rand()%screenHeight-SCOREBOARD_HEIGHT);
            enemy_X = -enemy.w() ;
        }//if

//----enemy chase----//
        if(enemy_X + enemy.w()/2 < fighter_X + fighter.w())
        {
            if(enemy_Y + enemy.h()/2 < fighter_Y )
                enemy_Y += EN_SPEED ;
            else if(enemy_Y + enemy.h()/2 > fighter_Y + fighter.h())
                enemy_Y -= EN_SPEED ;
        }//if

//----enemy attack----//
        if(enemy_X<=fighter_X&&enemy_X>=screenWidth/12)
            enemyATTACK() ;

        if(enemyIsATK)
        {
            if(enemyBullet_Y+enemyBullet.h()/2 < target_Y )
            {
                enemyBullet.blendTexRotate(enemyBullet_X, enemyBullet_Y,1.5,1,45) ;
                enemyBullet_Y += ENB_SPEED ;
            }
            else if(enemyBullet_Y + enemyBullet.h()/2 > target_Y + fighter.h() )
            {
                enemyBullet.blendTexRotate(enemyBullet_X, enemyBullet_Y,1.5,1,-45) ;
                enemyBullet_Y -= ENB_SPEED ;
            }
            else
            {
                enemyBullet.blendTex(enemyBullet_X, enemyBullet_Y) ;
            }
            enemyBullet_X += ENB_SPEED ;

            if(enemyBullet_X>=screenWidth)
            {
                enemyIsATK = false ;
            }
        }//if

//----collision----//
        enemyHited = !AABBtest(enemy_X,enemy_Y,enemy_X+enemy.w(),enemy_Y+enemy.h(),bullet_X,bullet_Y,bullet_X+bullet.w(),bullet_Y+bullet.h()) ;

        if(enemyHited)
        {
            enemyRemains -- ;
            enemy_X = 0-enemy.w() ;
            enemy_Y = (int)( rand()%( screenHeight-SCOREBOARD_HEIGHT-enemy.h() ) ) ;

            bullet_X = screenWidth ;
            bullet_Y = screenHeight ;

            enemyHited = false ;
            fire = false ;
        }//if

        fighterCrash = ! AABBtest(enemy_X,enemy_Y,enemy_X+enemy.w(),enemy_Y+enemy.h(),fighter_X,fighter_Y,fighter_X+fighter.w(),fighter_Y+fighter.h()) ;
        fighterHited = ! AABBtest(enemyBullet_X,enemyBullet_Y,enemyBullet_X+enemyBullet.w(),enemyBullet_Y+enemyBullet.h(),fighter_X,fighter_Y,fighter_X+fighter.w(),fighter_Y+fighter.h()) ;
        if(fighterCrash)
        {
            life-- ;
            enemy_X = 0-enemy.w() ;
            enemy_Y = (int)( rand()%( screenHeight-SCOREBOARD_HEIGHT-enemy.h() ) ) ;
            fighter_X = screenWidth-fighter.w() ;
            fighter_Y = (screenHeight-SCOREBOARD_HEIGHT)/2 - fighter.h()/2 ;
            fighterCrash = false ;
        }//if
        if(fighterHited)
        {
            life--;
            enemyBullet_X = screenWidth ;
            enemyBullet_Y = screenHeight ;
            fighter_X = screenWidth-fighter.w() ;
            fighter_Y = (screenHeight-SCOREBOARD_HEIGHT)/2 - fighter.h()/2 ;
            enemyIsATK = false ;
            fighterHited = false ;
        }//if

//----boundary detection of fighter----//
        if(fighter_X <=0)
            fighter_X = 0 ;
        if( fighter_X >= screenWidth-fighter.w() )
            fighter_X = screenWidth - fighter.w() ;
        if(fighter_Y <=0)
            fighter_Y = 0 ;
        if( fighter_Y >= screenHeight - SCOREBOARD_HEIGHT - fighter.h() )
            fighter_Y = screenHeight - SCOREBOARD_HEIGHT - fighter.h() ;

//----life check----//
        if(life <= 0)
            gameState = OVER ;
        if(enemyRemains<=0)
            gameState = WIN ;
        break ;

    case OVER :
        gameOver.blendTex(0,0) ;
        break ;

    case WIN :
        gameWin.blendTex(0,0) ;
        break;
    }//switch

    CheckError(__LINE__);
    glutSwapBuffers();
}//myDisplay()

void key_pressed(int key,int xx, int yy)
{
    switch(key)
    {
    case GLUT_KEY_UP:
        upPressed = true ;
        break ;
    case GLUT_KEY_DOWN:
        downPressed = true ;
        break ;
    case GLUT_KEY_LEFT:
        leftPressed = true ;
        break ;
    case GLUT_KEY_RIGHT:
        rightPressed = true ;
        break ;
    }
    glutPostRedisplay();
}//key pressed

void key_release(int key,int xx, int yy)
{
    switch(key)
    {
    case GLUT_KEY_UP:
        upPressed = false ;
        break;
    case GLUT_KEY_DOWN:
        downPressed = false ;
        break;

    case GLUT_KEY_LEFT:
        leftPressed = false ;
        break;
    case GLUT_KEY_RIGHT:
        rightPressed = false ;
        break;
    }
    glutPostRedisplay();
}//key_release()

void myKeyUp(unsigned char key,int xx, int yy)
{
    switch(key)
    {
    case ' ':
        if(fire==false)
        {
            bullet_X = fighter_X - 16 ;
            bullet_Y = fighter_Y + 17 ;
        }
        fire = true ;
        break ;
    case 's':
    case 'S':
        if(gameState==START)
        {
            gameState = RUN ;
        }
        break ;
    case 'r' :
    case 'R' :
        if(gameState==OVER||gameState == WIN)
        {
            gameState = START ;
            init() ;
        }
        break ;
    }//switch
}//myKeyUp()

void update(int i)
{
    double r = double(i)/50.0;

//    rectX = 400 + 250*sin(r);

//    rotation_test += 2;
//    scale_test += 0.01;

    ++i;
    glutTimerFunc( 16, update, i);
    glutPostRedisplay();
}//update()

void init()
{
/*
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
*/
//  cout<<"init()"<<endl;
//  glutKeyboardFunc(myKeys);
//	glutMouseFunc(myMouse);
//	glutMotionFunc(mouseMove);
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //background color(1.0, 1.0, 1.0): white color

    gameStart.readBMPFile("image/gamestart.bmp") ;
    gameOver.readBMPFile("image/gameover.bmp") ;
    gameWin.readBMPFile("image/win.bmp");
    bg[0].readBMPFile("image/bg1.bmp") ;
    bg[1].readBMPFile("image/bg2.bmp") ;
    fighter.readBMPFile("image/fighter.bmp") ;
    enemy.readBMPFile("image/enemy.bmp") ;
    bullet.readBMPFile("image/bullet.bmp") ;
    enemyBullet.readBMPFile("image/enemyBullet.bmp") ;
    enemy_icon.readBMPFile("image/enemy_small.bmp") ;
    fighter_icon.readBMPFile("image/fighter_small.bmp") ;

    gameState = START ;

    bg1X = 0 ;
    bg2X = -screenWidth ;

    fighter_X = screenWidth-fighter.w() ;
    fighter_Y = (screenHeight-SCOREBOARD_HEIGHT)/2 - fighter.h()/2 ;

    enemy_X = 0 -enemy.w() ;
    enemy_Y = (int)( rand()%( screenHeight-SCOREBOARD_HEIGHT-enemy.h() ) ) ;

    bullet_X = enemyBullet_X = screenWidth ;
    bullet_Y = enemyBullet_Y = screenHeight ;

    fire = upPressed = downPressed = leftPressed = rightPressed = enemyHited = fighterHited = enemyIsATK = fighterCrash = false ;

    enemy.setChromaKey(232, 248, 248);
    bullet.setChromaKey(232, 248, 248);
    fighter.setChromaKey(232, 248, 248);
    enemy_icon.setChromaKey(232, 248, 248);
    fighter_icon.setChromaKey(232, 248, 248);

    enemyRemains = 16 ;
    life = 3 ;
}//init()

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(50, 30);
    glutCreateWindow("2D World");

    srand(time(0));  //rand seed
    init();

    cout<<"Reading sprite";

    glutIgnoreKeyRepeat(1);
    glutSpecialFunc(key_pressed);
    glutSpecialUpFunc(key_release);

    glutKeyboardUpFunc(myKeyUp);
    /*
        pic[0].readBMPFile("image/stand.bmp");
        cout<<'.';
        pic[1].readBMPFile("image/walk.bmp");
        cout<<'.';
        pic[2].readBMPFile("image/fly.bmp");
        cout<<'.'<<endl;
    */
    enemy.setChromaKey(232, 248, 248);
    bullet.setChromaKey(232, 248, 248);
    fighter.setChromaKey(232, 248, 248);
    enemy_icon.setChromaKey(232, 248, 248);
    fighter_icon.setChromaKey(232, 248, 248);
    //cout<<"Reading Backgroud........"<<endl;
    //bg.readBMPFile("image/dog.bmp");

    // Initialize the time variables
    startTime = glutGet(GLUT_ELAPSED_TIME);
    prevTime = startTime;

    update(0);
    glutMainLoop();

    return 0;
}//main()
