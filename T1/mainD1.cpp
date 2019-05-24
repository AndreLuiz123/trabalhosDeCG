#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <iostream>
#include "extras.h"
#include <vector>
#include "Titulo.h"
#include <cmath>



using namespace std;

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.

class vertice
{
public:
    float x,y,z,espessura;
};

class triangle
{
public:
    vertice v[3];
};

class grupo
{
public:
    std::vector<vertice> verticesGrupo1;
    std::vector<vertice> verticesPerpendiculares;
};

/// Globals
float zdist = 5.0;
float rotationX = 0.0, rotationY = 0.0, posZ, posY;
int   last_x, last_y;
int   width, height;
int numeroPontos = 0;
int grupoAtual=0;
int altura=1;
float esp=0.5f;
int ultimaAltura;
bool apagarTela, telaCheia = false;
int indice=0;
FILE *arquivo = nullptr;


vertice initialVerticesV1 = {-1.0f, -1.0f,  0.0f};
vertice initialVerticesV2 = { 1.0f, -1.0f,  0.0f};
vertice initialVerticesV3 = {-1.0f,  1.0f,  0.0f};
vertice initialVerticesV4 = { 1.0f,  1.0f,  -0.5f};

vertice lastVertice1 = initialVerticesV1;
vertice lastVertice2 = initialVerticesV2;

std::vector<vertice> vert;
std::vector<triangle> triang;
std::vector<grupo> grupos;

grupo gr;

///TESTEEEEEEEEEEEEEEEEEEEEEEEE


/// Functions
Titulo *barraT = new Titulo((char*)"ALTURA: ", (char*)" GRUPO", altura, grupoAtual);

void init(void)
{
    //initLight(width, height); // Função extra para tratar iluminação.

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);                 // Habilita luz
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);
   // glFrontFace(GL_CW);
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    // glDisable(GL_CULL_FACE);
//    setMaterials();

    // Cor da fonte de luz (RGBA)
    GLfloat cor_luz[]     = { 1.0, 1.0, 1.0, 1.0};
    // Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
    GLfloat posicao_luz[] = { 50.0, 50.0, 50.0, 1.0};

    // Define parametros da luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    grupos.push_back(gr);


}

/* Exemplo de cálculo de vetor normal que são definidos a partir dos vértices do triângulo;
  v_2
  ^
  |\
  | \
  |  \       'vn' é o vetor normal resultante
  |   \
  +----> v_1
  v_0
*/
void CalculaNormal(triangle t, vertice *vn)
{
    vertice v_0 = t.v[0],
            v_1 = t.v[1],
            v_2 = t.v[2];
    vertice v1, v2;
    double len;

    /* Encontra vetor v1 */
    v1.x = v_1.x - v_0.x;
    v1.y = v_1.y - v_0.y;
    v1.z = v_1.z - v_0.z;

    /* Encontra vetor v2 */
    v2.x = v_2.x - v_0.x;
    v2.y = v_2.y - v_0.y;
    v2.z = v_2.z - v_0.z;

    /* Calculo do produto vetorial de v1 e v2 */
    vn->x = (v1.y * v2.z) - (v1.z * v2.y);
    vn->y = (v1.z * v2.x) - (v1.x * v2.z);
    vn->z = (v1.x * v2.y) - (v1.y * v2.x);

    /* normalizacao de n */
    len = sqrt(pow(vn->x,2) + pow(vn->y,2) + pow(vn->z,2));

    vn->x /= len;
    vn->y /= len;
    vn->z /= len;
}

vertice calculaPerpendicular(vertice v1, vertice v2, float h){

    float tangente, angulo;

    tangente = fabs(v1.y - v2.y)/fabs(v1.z - v2.z);

    if(v1.y>=v2.y)
    angulo = -1*(-3.14 -3.14/2 - atan(tangente));
    else
    angulo = (-3.14 -3.14/2 - atan(tangente));

    vertice vertNovo;

    vertNovo.z = 0;
    vertNovo.y = h*sin(angulo);
    vertNovo.x = h*cos(angulo);

    return vertNovo;
}

void drawObject()
{
    vertice vetorNormal;
    for(int j=0; j<grupos.size(); j++)
    {
        if(!grupos[j].verticesGrupo1.empty())
            for(int i=0; i<grupos[j].verticesGrupo1.size()-1; i++)
            {

                /*vertice v[4] = {
                {0.0f,  grupos[j].verticesGrupo1[i].y, grupos[j].verticesGrupo1[i].z},
                {grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].y, grupos[j].verticesGrupo1[i].z},
                {0.0f,  grupos[j].verticesGrupo1[i+1].y, grupos[j].verticesGrupo1[i+1].z},
                {grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].y, grupos[j].verticesGrupo1[i+1].z}
                               };*/

                vertice v[4] = {
                {grupos[j].verticesGrupo1[i].z+grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].y+grupos[j].verticesPerpendiculares[i].y, 0},
                {grupos[j].verticesGrupo1[i].z-grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].y-grupos[j].verticesPerpendiculares[i].y, 0},
                {grupos[j].verticesGrupo1[i+1].z+grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesPerpendiculares[i+1].y, 0},
                {grupos[j].verticesGrupo1[i+1].z-grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].y-grupos[j].verticesPerpendiculares[i+1].y, 0}
                               };
                 vertice v2[4] = {
                {grupos[j].verticesGrupo1[i].z+grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].y+grupos[j].verticesPerpendiculares[i].y,grupos[j].verticesGrupo1[i].espessura},
                {grupos[j].verticesGrupo1[i].z-grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].y-grupos[j].verticesPerpendiculares[i].y,grupos[j].verticesGrupo1[i].espessura},
                {grupos[j].verticesGrupo1[i+1].z+grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesPerpendiculares[i+1].y,grupos[j].verticesGrupo1[i+1].espessura},
                {grupos[j].verticesGrupo1[i+1].z-grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].y-grupos[j].verticesPerpendiculares[i+1].y,grupos[j].verticesGrupo1[i+1].espessura}
                               };

                triangle t[2] = {{ v[2], v[1],v[0]},
                    {v[2] , v[3],v[1]}
                };

                 triangle t2[2] = {{ v2[0], v2[1],v2[2]},
                    {v2[1], v2[3],v2[2] }
                };

                 triangle t3[2] = {{ v[0], v2[0],v2[2]},
                    {v2[2] , v[2],v[0]}
                };

                  triangle t4[2] = {{v2[3], v2[1],v[1] },
                    {v[1], v[3], v2[3]}
                };

                   triangle t5[2] = {{v2[1], v2[0],v[0]},
                    {v[0], v[1], v2[1]}
                };

                 triangle t6[2] = { {v[2], v2[2], v2[3]},
                    {v2[3], v[3],v[2]}
                };

               // CalculaNormal(t[i], &vetorNormal);

                ///o que é o 3D?-1
            /*  vertice v2[4] = {
                {0.0f+vetorNormal.x,  grupos[j].verticesGrupo1[i].y+vetorNormal.y, grupos[j].verticesGrupo1[i].z+vetorNormal.z},
                {grupos[j].verticesGrupo1[i].x+vetorNormal.x,  grupos[j].verticesGrupo1[i].y+vetorNormal.y, grupos[j].verticesGrupo1[i].z+vetorNormal.z},
                {0.0f+vetorNormal.x,  grupos[j].verticesGrupo1[i+1].y+vetorNormal.y, grupos[j].verticesGrupo1[i+1].z+vetorNormal.z},
                {grupos[j].verticesGrupo1[i+1].x+vetorNormal.x,  grupos[j].verticesGrupo1[i+1].y+vetorNormal.y, grupos[j].verticesGrupo1[i+1].z+vetorNormal.z}
                               };



                ///o que é o 3D?-2
                vertice v2[4] = {
                {0.0f, grupos[j].verticesGrupo1[i].y+grupos[j].verticesGrupo1[i].espessura, grupos[j].verticesGrupo1[i].z},
                {grupos[j].verticesGrupo1[i].x, grupos[j].verticesGrupo1[i].y+grupos[j].verticesGrupo1[i].espessura, grupos[j].verticesGrupo1[i].z},
                {0.0f, grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesGrupo1[i+1].espessura, grupos[j].verticesGrupo1[i+1].z},
                {grupos[j].verticesGrupo1[i+1].x, grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesGrupo1[i+1].espessura, grupos[j].verticesGrupo1[i+1].z}
                                };

                triangle t2[2] = {{v2[0], v2[1], v2[2]},
                    {v2[1], v2[3], v2[2]}
                };

                triangle t3[2] = {{v2[0], v2[1], v[0]},
                    {v2[1], v[1], v[0]}
                };

                triangle t4[2] = {{v2[3], v2[2], v[3]},
                    {v2[2], v[2], v[3]}
                };

                triangle t5[2] = {{v2[2], v2[0], v[0]},
                    {v2[2], v[2], v[0]}
                };

                triangle t6[2] = {{v2[3], v2[1], v[1]},
                    {v2[3], v[3], v[1]}
                };
*/


                glBegin(GL_TRIANGLES);
                for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t[i].v[j].x, t[i].v[j].y, t[i].v[j].z);
                }
             /**/    for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t2[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t2[i].v[j].x, t2[i].v[j].y, t2[i].v[j].z);
                }

                for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t3[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t3[i].v[j].x, t3[i].v[j].y, t3[i].v[j].z);
                }

                for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t4[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t4[i].v[j].x, t4[i].v[j].y, t4[i].v[j].z);
                }
                 for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t5[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t5[i].v[j].x, t5[i].v[j].y, t5[i].v[j].z);
                }
/**/                    for(int i = 0; i < 2; i++) // triangulos
                {
                    CalculaNormal(t6[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                    for(int j = 0; j < 3; j++) // vertices do triangulo
                        glVertex3d(t6[i].v[j].x, t6[i].v[j].y, t6[i].v[j].z);
                }/**/
                glEnd();

            }
    }


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

void desenho2D()
{
    glDisable(GL_LIGHTING);
    glPointSize(9.0f);
    glEnable(GL_POINT_SMOOTH);
    glColor3b(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for(int j = 0; j<grupos.size(); j++)
    {
        if(!grupos[j].verticesGrupo1.empty())
        {
            for(int i = 0; i<grupos[j].verticesGrupo1.size(); i++)
                glVertex2d(grupos[j].verticesGrupo1[i].z, grupos[j].verticesGrupo1[i].y);
        }
    }


    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glEnable(GL_LIGHTING);
}

void display(void)
{


    ///A primeira metade é aquela na qual existe o desenho do gráfico
    ///O scissor é ativado para fazer os desenhos nessa metade
    glViewport(0,0, width/2, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width/2, height);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    desenhaEixos();
    desenho2D();
    glDisable(GL_SCISSOR_TEST);
    ///A segunda metade é aquela na qual existe a imagem 3D
    ///O scissor é ativado para fazer os desenhos nessa metade



    glViewport(width/2,0, width/2, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) width*0.5f/(GLfloat) height, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();
    glRotatef( rotationY, 0.0, 1.0, 0.0 );
    glRotatef( rotationX, 1.0, 0.0, 0.0 );

    //if(grupos[grupoAtual].verticesGrupo1.size()>1)
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    drawObject();

   // quadric = gluNewQuadric();

    //gluQuadricDrawStyle(quadric, GLU_FILL );
    //gluSphere( quadric , .5 , 36 , 18 );


    glPopMatrix();

    apagarTela = false;

    barraT->setAltura(altura);
    barraT->setGrupo(grupoAtual);
    barraT->alteraTitulo();


    glutSwapBuffers();
}

void idle ()
{
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    width = w;
    height = h;

    glViewport (0, 0, (GLsizei) w/2, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w*0.5f/(GLfloat) h, 0.01, 200.0);
}

void keyboard (unsigned char key, int x, int y)
{

    switch (tolower(key))
    {
    case 27:
        exit(0);
        break;
    }

    switch (key)
    {
    case '.':
        esp+=0.5f;
        break;
    case ',':
        if(esp>0)
        esp-=0.5f;
        break;
    }
}

void specialKeysPress (int key, int x, int y)
{


    switch (tolower(key))
    {
    case GLUT_KEY_UP:
        altura++;
        break;
    case GLUT_KEY_DOWN:
        if(altura>0)
        altura--;
        break;
    case GLUT_KEY_RIGHT:

        indice++;

        if(indice>grupos.size()-1)
        {
            grupo gr;
            grupos.push_back(gr);
            grupoAtual=indice;
        }
        else
        {
            grupoAtual=indice;
        }

        break;
    case GLUT_KEY_LEFT:
        indice--;
        if(indice<0)
        {
            indice = 0;
            grupoAtual=0;
        }
        else
        {
            grupoAtual = indice;
        }
        break;
    case GLUT_KEY_F12:
        if(!telaCheia){
            glutFullScreen();
            telaCheia = true;
        }else{
            glutReshapeWindow(800,600);
            glutPositionWindow(0,0);
            telaCheia = false;
        }

        break;

    }
}

// Motion callback
void motion(int x, int y )
{
    rotationX += (float) (y - last_y);
    rotationY += (float) (x - last_x);

    last_x = x;
    last_y = y;
}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        last_x = x;
        last_y = y;
        posZ = ((float)x*4)/width -1;
        posY = -(((float)y*2)/height -1);
        if(posZ<1)
        {
            vertice newVertice;

            newVertice.espessura = esp;
            newVertice.x = altura;
            newVertice.y = posY;
            newVertice.z = posZ;
            grupos[grupoAtual].verticesGrupo1.push_back(newVertice);

            if(grupos[grupoAtual].verticesGrupo1.size()>=2){
            vertice verticeAux;

            if(grupos[grupoAtual].verticesGrupo1.size()==2){
            verticeAux = calculaPerpendicular(grupos[grupoAtual].verticesGrupo1[grupos[grupoAtual].verticesGrupo1.size()-1], grupos[grupoAtual].verticesGrupo1[grupos[grupoAtual].verticesGrupo1.size()-2],0.25f);
            grupos[grupoAtual].verticesPerpendiculares.push_back(verticeAux);
            grupos[grupoAtual].verticesPerpendiculares.push_back(verticeAux);
            }else{
            verticeAux = calculaPerpendicular(grupos[grupoAtual].verticesGrupo1[grupos[grupoAtual].verticesGrupo1.size()-1], grupos[grupoAtual].verticesGrupo1[grupos[grupoAtual].verticesGrupo1.size()-2],0.25f);
            grupos[grupoAtual].verticesPerpendiculares.push_back(verticeAux);
            }


            }

            cout<<newVertice.espessura <<endl;
            //grupos[grupoAtual].verticesGrupo2.push_back(newVertice);

        }
        // drawNewObject();

    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        if(grupos[grupoAtual].verticesGrupo1.size()>0)
        {


            if(grupos[grupoAtual].verticesGrupo1.size()<=2){
            if(grupos[grupoAtual].verticesGrupo1.size()==2){
            grupos[grupoAtual].verticesPerpendiculares.pop_back();
            grupos[grupoAtual].verticesPerpendiculares.pop_back();
            grupos[grupoAtual].verticesGrupo1.pop_back();
            }
            if(grupos[grupoAtual].verticesGrupo1.size()==1){
            grupos[grupoAtual].verticesGrupo1.pop_back();
            }
            }else{
            grupos[grupoAtual].verticesGrupo1.pop_back();
            grupos[grupoAtual].verticesPerpendiculares.pop_back();
            }




        }

    }

    if(button == 3) // Scroll up
    {
        zdist+=1.0;
    }
    if(button == 4) // Scroll Down
    {
        zdist-=1.0;
    }
}

/// Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("Trabalho de CG");
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeysPress);
    glutIdleFunc(idle);
    desenhaEixos();
    desenho2D();
    glutMainLoop();
    return 0;
}
