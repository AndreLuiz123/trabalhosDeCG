/*
Code adapted from the one available here: http://nghiaho.com/?p=1613

W,A,S,D - to move
mouse - look around, inverted mouse
ESC - quit

*/
#include <iostream>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <vector>

#include "camera.h"

float zdist = 5.0;
float rotationX = 0.0, rotationY = 0.0, posZ, posY,posZ1, posY1,posZ2, posY2;
int   last_x, last_y;
int   width, height;
int numeroPontos = 0;
int grupoAtual=0;
int altura=1;
int ultimaAltura;
bool apagarTela;
int indice=0;

bool modoNavegador = true;

class vertice
{
    public:
        float x,y,z;
};

std::vector<vertice> vertices;

class grupo
{
    public:
        std::vector<vertice> verticesGrupo;
};


void Display();
void Reshape (int w, int h);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void MouseMotion(int x, int y);
void Mouse(int button, int state, int x, int y);
void Timer(int value);
void Idle();
void scene();
void setMaterial(void);
void init (void);
void Grid();

Camera g_camera;
bool 	g_key[256];
bool 	g_shift_down = false;
int 	g_viewport_width = 0;
int 	g_viewport_height = 0;
bool 	g_mouse_left_down = false;
bool	g_mouse_right_down = false;
bool	fullscreen = false;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool 	inverseMouse = false;
bool	boostSpeed = false; // Change keyboard speed
bool  flyMode = false;
bool	releaseMouse = false;

// Movement settings
float g_translation_speed = 0.05;
float g_rotation_speed = M_PI/180*0.2;
float initialY = 2; // initial height of the camera (flymode off value)

void setMaterial(void)
{
	// Material do objeto (neste caso, ruby). Parametros em RGBA
	GLfloat objeto_ambient[]   = { .1745, .01175, .01175, 1.0 };
	GLfloat objeto_difusa[]    = { .91424, .04136, .04136, 1.0 };
	GLfloat objeto_especular[] = { .727811, .626959, .626959, 1.0 };
	GLfloat objeto_brilho[]    = { 90.0f };

	// Define os parametros da superficie a ser iluminada
	glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
	glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
	glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);
}

void scene()
{
    glViewport(0 ,0, g_viewport_width, g_viewport_height);
	float size = 110.0f;
	glDisable(GL_LIGHTING);
	glColor3f(0.9f, 0.5f, 0.0f);
	glPushMatrix();
		glScalef(size, .1, size);
		glutSolidCube(1);
	glPopMatrix();
	glColor3f(0.8f, 1.0f, 0.8f);
	/*glPushMatrix();
		glTranslatef(0.0f, 20.0f, 0.0f);
		glScalef(size, .2, size);
		glutSolidCube(1);
	glPopMatrix();*/

	glEnable(GL_LIGHTING);
	setMaterial();
	/*for(int j = -50; j <= 50; j+=10)
	{
		for(int i = -50; i <= 50; i+=10)
		{
			glPushMatrix();
			glTranslated(i, 10, j);
			glScalef(1.5, 20, 1.5);
			glutSolidCube(1);
			glPopMatrix();
		}
	}*/
	///Desenhar paredes aqui
	glDisable(GL_LIGHTING);
}

void desenhaEixos()
{
    glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glColor3f(1,0,0);
            glVertex2d(-1,0);
            glVertex2d(1,0);
            glColor3f(0,1,0);
            glVertex2d(0,-1);
            glVertex2d(0,1);
        glEnd();
        glEnable(GL_LIGHTING);
}

void desenhaVertices()
{
    glDisable(GL_LIGHTING);
            glPointSize(9.0f);
            glEnable(GL_POINT_SMOOTH);
            glColor3b(0.0, 0.0, 0.0);
            glBegin(GL_POINTS);
                    if(!vertices.empty()){
                    for(int i = 0; i<vertices.size(); i++)
                        glVertex2d(vertices[i].x, vertices[i].y);
                    }



            glEnd();
    glEnable(GL_LIGHTING);
}

void wallCreator()
{

/// ------------------------2D---------------------------------
    glViewport(0 ,0, g_viewport_width/2, g_viewport_height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);
        glScissor(0, 0, g_viewport_width/2, g_viewport_height);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1,1,-1,1,-1,1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        desenhaEixos();
/// ------------------------2D---------------------------------
/// ------------------------3D---------------------------------

    glViewport(g_viewport_width/2 ,0, g_viewport_width/2, g_viewport_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) g_viewport_width*0.5f/(GLfloat) g_viewport_height, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (0.0, 0.0, 10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef( rotationY, 0.0, 1.0, 0.0 );
    glRotatef( rotationX, 1.0, 0.0, 0.0 );
    glColor3f(1,0,0);
    GLUquadric *quad;
    quad = gluNewQuadric();
    gluSphere(quad,5,20,4);
/// ------------------------3D-----------------------------

}

void Display (void)
{
	glClearColor (0.0,0.0,0.0,1.0); //clear the screen to black
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color buffer and the depth buffer
	glLoadIdentity();

	g_camera.Refresh();
	glColor3f(0,1,0);

	// Draw a scene
	if(modoNavegador)
	{
      	scene();
	}else
	{
        wallCreator();
	}

	glutSwapBuffers(); //swap the buffers
}

void Reshape (int w, int h)
{
	g_viewport_width = w;
	g_viewport_height = h;

	glViewport (0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
	glMatrixMode (GL_PROJECTION); //set the matrix to projection

	glLoadIdentity ();
	gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.1 , 1000.0); //set the perspective (angle of sight, width, height, ,depth)
	glMatrixMode (GL_MODELVIEW); //set the matrix back to model
}

void KeyboardUp(unsigned char key, int x, int y)
{
	g_key[key] = false;
}

void Timer(int value)
{
	float speed = g_translation_speed;

	if(g_key['w'] || g_key['W'])
	{
		g_camera.Move(speed, flyMode);
	}
	else if(g_key['s'] || g_key['S'])
	{
		g_camera.Move(-speed, flyMode);
	}
	else if(g_key['a'] || g_key['A'])
	{
		g_camera.Strafe(speed);
	}
	else if(g_key['d'] || g_key['D'])
	{
		g_camera.Strafe(-speed);
	}

	glutTimerFunc(1, Timer, 0);
}

void Idle()
{
	Display();
}

void Mouse(int button, int state, int x, int y)
{

     if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
     {
            vertice v;

            v.x = x;
            v.y = y;

            vertices.push_back(v);

     }



}

void MouseMotion(int x, int y)
{
	// This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
	// This avoids it being called recursively and hanging up the event loop
	static bool just_warped = false;

	if(just_warped)
	{
		just_warped = false;
		return;
	}

	int dx = x - g_viewport_width/2;
	int dy = y - g_viewport_height/2;

	if(inverseMouse) dy = g_viewport_height/2-y;

	if(dx) g_camera.RotateYaw(g_rotation_speed*dx);
	if(dy) g_camera.RotatePitch(g_rotation_speed*dy);

	if(!releaseMouse)	glutWarpPointer(g_viewport_width/2, g_viewport_height/2);

	just_warped = true;
}

void pressKey(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_F11 :
			fullscreen = !fullscreen;
			(fullscreen) ? glutFullScreen() : glutReshapeWindow(800,600);
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(tolower(key))
	{
		case 27:
			exit(0);
		break;
		case 'i':
			inverseMouse = !inverseMouse;
			inverseMouse ? printf("InverseMouse ON\n") : printf("InverseMouse OFF\n");
		break;
		case 'b':
			boostSpeed = !boostSpeed;
			(boostSpeed) ? g_translation_speed = 0.2 : g_translation_speed = 0.05;
			boostSpeed ? printf("BoostMode ON\n") : printf("BoostMode OFF\n");
		break;
		case 'f':
			flyMode = !flyMode;
			if(flyMode)
			{
				printf("FlyMode ON\n");
			}
			else
			{
				float x, y, z;
				printf("FlyMode OFF\n");
				g_camera.GetPos(x, y, z);
				g_camera.SetPos(x, initialY, z);
			}

		break;
		case 'r':
			releaseMouse = !releaseMouse;
			releaseMouse ? printf("Mouse released\n") : printf("Mouse Attached\n");
		break;
        case 'n':
            if(modoNavegador)
			modoNavegador = false;
			else
			modoNavegador = true;
		break;


	}
	g_key[key] = true;
}


// Init callback
void init (void)
{
	float pos[3] = {0.0f, 2.0f, 5.0f};
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
	glEnable(GL_LIGHTING);                 // Habilita luz
	glEnable(GL_LIGHT0);                   // habilita luz 0

	// Cor da fonte de luz (RGBA)
	GLfloat cor_luz[]         = { 1.0, 1.0, 1.0, 1.0};
	// Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
	GLfloat posicao_luz[]     = { 50.0, 50.0, 50.0, 1.0};

	// Define parametros da luz
	glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
	glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
	glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

	glutSetCursor(GLUT_CURSOR_NONE);

	printf("NAVIGATOR - First Person Shooter (FPS) Example\n\n");
	printf("  - Press F11 to enable FullScreen\n");
	printf("  - Press 'w', 'a', 's', 'd' and mouse to move\n");
	printf("  - Press 'i' to inverse mouse (y direction)\n");
	printf("  - Press 'b' to increase/reduce speed\n");
	printf("  - Press 'f' to enable/disable Fly Mode\n");
	printf("  - Press 'r' to release/attach mouse cursor\n\n");

	g_camera.SetPos(pos[0], pos[1], pos[2]);
}

int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("NAVIGATOR - F11 for FULLSCREEN");

	glutIgnoreKeyRepeat(1);

	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(pressKey);
	glutIdleFunc(Idle);
	glutTimerFunc(1, Timer, 0);

	init();

	glutMainLoop();

	return 0;
}
