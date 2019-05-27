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
#include "extrasD1.h"
#include "camera.h"
#include <fstream>
#define M_PI   3.14159265358979323846264338327950288
#define diminui -14
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
//    std::vector<objeto> objetosPly;
};

class face
{
public:
    int nVertices;
    std::vector<vertice> verticesFace;
    vertice vetorNormal;
};

class objeto
{
    public:
    float posY, posX;
    float zdist, escala;
    std::vector<face> facesImg;
    float material[13], orientacao, orientacaoEmPe=0;
    int nTriang=0;
    float alturaDescida = -11;
};

/// Globals
float zdist = 5.0, olhoY = 5, olhoX = 0, olhoZ = 10;
float rotationX = 272, rotationY = 0.0, posZ, posY, novoAng;
char ply;
int   last_x, last_y;
int   width, height;
int numeroPontos = 0;
int grupoAtual=0;
int altura=1;
float esp=0.5f;
int ultimaAltura;
bool apagarTela, telaCheia = false, modoNavegacao = true, modoPly = false;
int indice=0;
FILE *arquivo = nullptr;
 Camera cam;

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
float g_translation_speed = 0.5;
float g_rotation_speed = M_PI/180*0.2;
float initialY = 2; // initial height of the camera (flymode off value)

std::ifstream arquivoPly;
std::string str;
int numeroVertices;
int numeroFaces;
int numeroPropriedades = -1;
std::vector<vertice> verticesImg;
std::vector<face> facesImg;
std::vector<objeto> objetos;
bool wireframe = false;
float maiorY=-1000;
float menorY=10000;
float maiorX=-1000;
float menorX=1000;
float maiorZ=-1000;



///TESTEEEEEEEEEEEEEEEEEEEEEEEE


/// Functions
Titulo *barraT = new Titulo((char*)"ALTURA: ", (char*)" GRUPO", altura, grupoAtual);
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

void CalculaNormalPly(face t, vertice *vn)
{
    vertice v_0 = t.verticesFace[0],
            v_1 = t.verticesFace[1],
            v_2 = t.verticesFace[2];
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
///------------------------------------------------MOVIMENTO DO MOUSE----------------------------------------------
void KeyboardUp(unsigned char key, int x, int y)
{
	g_key[key] = false;
}
void Timer()
{
	float speed = g_translation_speed;

	if(g_key['w'] || g_key['W'])
	{
		cam.Move(speed, flyMode);
	}
	else if(g_key['s'] || g_key['S'])
	{
		cam.Move(-speed, flyMode);
	}
	else if(g_key['a'] || g_key['A'])
	{
		cam.Strafe(speed);
	}
	else if(g_key['d'] || g_key['D'])
	{
		cam.Strafe(-speed);
	}

	//glutTimerFunc(1, Timer);
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

	if(dx) cam.RotateYaw(g_rotation_speed*dx);
	if(dy) cam.RotatePitch(g_rotation_speed*dy);

	if(!releaseMouse)	glutWarpPointer(g_viewport_width/2, g_viewport_height/2);

	just_warped = true;
}
///------------------------------------------------MOVIMENTO DO MOUSE----------------------------------------------
///------------------------------------------------------------D2---------------------------------------------------------------

void leArquivoPly(int x, int y, float ang, char ply)
{

    objeto obj;
    std::ifstream arquivo;

    switch(ply)
    {
        case 'v':
            cout<<"vaca"<<endl;
            obj.orientacaoEmPe = 0;
            obj.escala = 2.5;
            obj.alturaDescida = -17;
            arquivo.open("cow.ply");
        break;
        case 'V':
            cout<<"vaca"<<endl;
            obj.orientacaoEmPe = 0;
            obj.escala = 2.5;
            obj.alturaDescida = -17;
            arquivo.open("cow.ply");

        break;
        case 'f':
            cout<<"formiga"<<endl;
            obj.orientacaoEmPe = 0;
            obj.escala = 0.1;
            obj.alturaDescida = -16;
            arquivo.open("ant.ply");
        break;
        case 'F':
            cout<<"formiga"<<endl;
            obj.orientacaoEmPe = 0;
            obj.escala = 0.1;
            obj.alturaDescida = -16;
            arquivo.open("ant.ply");
        break;
        case 'b':
            cout<<"boneco de neve"<<endl;
            obj.orientacaoEmPe = -90;
            obj.escala = 1;
            obj.alturaDescida = -16;
            arquivo.open("snowman.ply");
        break;
        case 'B':
            cout<<"boneco de neve"<<endl;
            obj.orientacaoEmPe = -90;
             obj.escala = 1;
            obj.alturaDescida = -16;
             arquivo.open("snowman.ply");
        break;
    }

    bool leArquivo=true;


    while(leArquivo)
    {//cout<<str<<endl;
        arquivo >> str;
        if(str=="property")
            numeroPropriedades++;
        else
        {

            if(str=="vertex")
                arquivo >> numeroVertices;
            else
            {
                if(str=="face")
                    arquivo >> numeroFaces;
                else
                {
                    if(str=="end_header")
                        leArquivo = false;
                }
            }

        }
    }



    for(int  i = 0; i<numeroVertices; i ++)
    {
        vertice v;
        float ignorar;
        arquivo >> v.x;
        arquivo >> v.y;
        arquivo >> v.z;

        for(int j=0; j<numeroPropriedades-3;j++)
            arquivo >> ignorar;
         if(v.y>maiorY)
            maiorY = v.y;
         if(v.y<menorY)
            menorY = v.y;
         if(v.x>maiorX)
            maiorX = v.x;
         if(v.x<menorX)
            menorX = v.x;
         if(v.z>maiorZ)
            maiorZ = v.z;

        verticesImg.push_back(v);
    }

    for(int  i = 0; i<numeroFaces; i ++)
    {
        face f;
        arquivo >> f.nVertices;
        if(f.nVertices == 3)
        obj.nTriang++;
        for(int j=0; j<f.nVertices; j++)
        {
            int indice;
            arquivo >> indice;
            f.verticesFace.push_back(verticesImg[indice]);
        }
        CalculaNormalPly(f, &f.vetorNormal);
        //cout<<obj.facesImg.size()<<endl;
        obj.facesImg.push_back(f);
    }
    cout<<obj.facesImg.size()<<endl;

    obj.posY = y;
    obj.posX = x;
    obj.orientacao = ang;
    cout<<obj.posX<<" e "<<obj.posY<<endl;
    objetos.push_back(obj);
    obj.zdist = abs(maiorY - obj.posY)/tan(3.14/9);
    obj.facesImg = facesImg;
    facesImg.clear();
    verticesImg.clear();
    numeroPropriedades = -1;
    menorY = 0;
    maiorY = 0;
    maiorZ = 0;
}

void drawObjectPly(int n)
{


      //  glRotatef(90,1,0,0);
        for(int i = 0; i < objetos[n].facesImg.size(); i++)
        {
            //CalculaNormalPly(facesImg[i], &vetorNormal);
            glNormal3f(objetos[n].facesImg[i].vetorNormal.x, objetos[n].facesImg[i].vetorNormal.y,objetos[n].facesImg[i].vetorNormal.z);
            if(!wireframe){
            glBegin(GL_POLYGON);
            for(int j=0; j < objetos[n].facesImg[i].nVertices; j++)
            {
                glVertex3d(objetos[n].facesImg[i].verticesFace[j].x,objetos[n].facesImg[i].verticesFace[j].y,objetos[n].facesImg[i].verticesFace[j].z);
            }
            glEnd();
            }else{

            glBegin(GL_LINE_STRIP);
            for(int j=0; j < objetos[n].facesImg[i].nVertices; j++)
            {
                glVertex3d(objetos[n].facesImg[i].verticesFace[j].x,objetos[n].facesImg[i].verticesFace[j].y,objetos[n].facesImg[i].verticesFace[j].z);
            }
            glEnd();

            }
        }

}

///------------------------------------------------------------D2---------------------------------------------------------------
void init(void)
{
    //initLight(width, height); // Função extra para tratar iluminação.

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);                 // Habilita luz
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
   // glFrontFace(GL_CW);
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    // glDisable(GL_CULL_FACE);
    setMaterials();

    // Cor da fonte de luz (RGBA)
    GLfloat cor_luz[]     = { 1.0, 1.0, 1.0, 1.0};
    // Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
    GLfloat posicao_luz[] = { 50.0, 50.0, 50.0, 1.0};

    // Define parametros da luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    cam.SetPos(0,-13,0);

    grupos.push_back(gr);
    ifstream arqPly;
    /*arqPly.open("snowman.ply");
    arquivoPly.open("ant.ply");
    leArquivoPly( 0,0,90,'f');
    leArquivoPly(arquivoPly, 0,2,90,'v');*/

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

                vertice v[4] = {
                {grupos[j].verticesGrupo1[i].z+grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  diminui, grupos[j].verticesGrupo1[i].y+grupos[j].verticesPerpendiculares[i].y},
                {grupos[j].verticesGrupo1[i].z-grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  diminui, grupos[j].verticesGrupo1[i].y-grupos[j].verticesPerpendiculares[i].y},
                {grupos[j].verticesGrupo1[i+1].z+grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x, diminui , grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesPerpendiculares[i+1].y},
                {grupos[j].verticesGrupo1[i+1].z-grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x, diminui,grupos[j].verticesGrupo1[i+1].y-grupos[j].verticesPerpendiculares[i+1].y}
                               };
                 vertice v2[4] = {
                {grupos[j].verticesGrupo1[i].z+grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].espessura+diminui,grupos[j].verticesGrupo1[i].y+grupos[j].verticesPerpendiculares[i].y},
                {grupos[j].verticesGrupo1[i].z-grupos[j].verticesPerpendiculares[i].x*grupos[j].verticesGrupo1[i].x,  grupos[j].verticesGrupo1[i].espessura+diminui, grupos[j].verticesGrupo1[i].y-grupos[j].verticesPerpendiculares[i].y},
                {grupos[j].verticesGrupo1[i+1].z+grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].espessura+diminui,grupos[j].verticesGrupo1[i+1].y+grupos[j].verticesPerpendiculares[i+1].y},
                {grupos[j].verticesGrupo1[i+1].z-grupos[j].verticesPerpendiculares[i+1].x*grupos[j].verticesGrupo1[i+1].x,  grupos[j].verticesGrupo1[i+1].espessura+diminui,grupos[j].verticesGrupo1[i+1].y-grupos[j].verticesPerpendiculares[i+1].y}
                               };


                /*triangle t[2] = {{ v[2], v[1],v[0]},
                    {v[2] , v[3],v[1]}
                };*/

                triangle t[2],t2[2],t3[2],t4[2],t5[2],t6[2];


                if(grupos[j].verticesGrupo1[i+1].y > grupos[j].verticesGrupo1[i].y )
                {
                    t[0].v[0] = v[2];
                    t[0].v[1] = v[1];
                    t[0].v[2] = v[0];

                    t[1].v[0] = v[2];
                    t[1].v[1] = v[3];
                    t[1].v[2] = v[1];

                    t2[0].v[0] = v2[0];
                    t2[0].v[1] = v2[1];
                    t2[0].v[2] = v2[2];

                    t2[1].v[0] = v2[1];
                    t2[1].v[1] = v2[3];
                    t2[1].v[2] = v2[2];

                    t3[0].v[0] = v[0];
                    t3[0].v[1] = v2[0];
                    t3[0].v[2] = v2[2];

                    t3[1].v[0] = v2[2];
                    t3[1].v[1] = v[2];
                    t3[1].v[2] = v[0];

                    t4[0].v[0] = v2[3];
                    t4[0].v[1] = v2[1];
                    t4[0].v[2] = v[1];

                    t4[1].v[0] = v[1];
                    t4[1].v[1] = v[3];
                    t4[1].v[2] = v2[3];

                    t5[0].v[0] = v2[1];
                    t5[0].v[1] = v2[0];
                    t5[0].v[2] = v[0];

                    t5[1].v[0] = v[0];
                    t5[1].v[1] = v[1];
                    t5[1].v[2] = v2[1];

                    t6[0].v[0] = v[2];
                    t6[0].v[1] = v2[2];
                    t6[0].v[2] = v2[3];

                    t6[1].v[0] = v2[3];
                    t6[1].v[1] = v[3];
                    t6[1].v[2] = v[2];
                }else
                {
                    t[0].v[0] =v[0];
                    t[0].v[1] = v[1];
                    t[0].v[2] =  v[2];

                    t[1].v[0] = v[1];
                    t[1].v[1] = v[3];
                    t[1].v[2] = v[2];

                    t2[0].v[0] = v2[2];
                    t2[0].v[1] = v2[1];
                    t2[0].v[2] = v2[0];

                    t2[1].v[0] = v2[2];
                    t2[1].v[1] = v2[3];
                    t2[1].v[2] = v2[1];

                    t3[0].v[0] = v2[2];
                    t3[0].v[1] = v2[0];
                    t3[0].v[2] = v[0];

                    t3[1].v[0] = v[0];
                    t3[1].v[1] = v[2];
                    t3[1].v[2] = v2[2];

                    t4[0].v[0] = v[1];
                    t4[0].v[1] = v2[1];
                    t4[0].v[2] = v2[3];

                    t4[1].v[0] = v2[3];
                    t4[1].v[1] = v[3];
                    t4[1].v[2] = v[1];

                    t5[0].v[0] = v[0];
                    t5[0].v[1] = v2[0];
                    t5[0].v[2] = v2[1];

                    t5[1].v[0] = v2[1];
                    t5[1].v[1] = v[1];
                    t5[1].v[2] = v[0];

                    t6[0].v[0] = v2[3];
                    t6[0].v[1] = v2[2];
                    t6[0].v[2] = v[2];

                    t6[1].v[0] = v[2];
                    t6[1].v[1] = v[3];
                    t6[1].v[2] = v2[3];

                }



              /*  triangle t[2] = {{ v[0], v[1],v[2]},
                    {v[1], v[3],v[2] }
                };

                 triangle t2[2] = {{ v2[2], v2[1],v2[0]},
                    {v2[2] , v2[3],v2[1]}
                };

                 triangle t3[2] = {{ v2[2], v2[0],v[0]},
                    {v[0] , v[2],v2[2]}
                };

                  triangle t4[2] = {{v[1], v2[1],v2[3] },
                    {v2[3], v[3], v[1]}
                };

                   triangle t5[2] = {{v[0], v2[0],v2[1]},
                    {v2[1], v[1],v[0] }
                };

                 triangle t6[2] = { { v2[3], v2[2],v[2]},
                    {v[2], v[3],v2[3]}
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
    glColor3f(30,0,0);
    glVertex2d(-30,0);
    glVertex2d(30,0);
    glColor3f(0,30,0);
    glVertex2d(0,-30);
    glVertex2d(0,30);
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

        if(!objetos.empty())
        {
            for(int i = 0; i<objetos.size(); i++)
                glVertex2d(objetos[i].posX, objetos[i].posY);
        }

    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glEnable(GL_LIGHTING);
}

void wallCreator()
{
  glViewport(0,0, width/2, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width/2, height);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-30,30,-30,30,-1,1);
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
    for(int n = 0; n< objetos.size(); n++)
    {
         glPushMatrix();
            //glRotated(90, 0.0,1.0,0.0);
            //cout<<objetos[n].facesImg[0].vetorNormal.x<<endl;
          //  glTranslated(objetos[n].posX,objetos[n].alturaDescida,objetos[n].posY);
            //glTranslated(objetos[n].posX,objetos[n].alturaDescida,objetos[n].posY);
            glScalef(objetos[n].escala,objetos[n].escala,objetos[n].escala);
            glRotatef(objetos[n].orientacao,0.0,1.0,0.0);
            glRotatef(objetos[n].orientacaoEmPe,1.0,0.0,0.0);
            drawObjectPly(n);
            glPopMatrix();
    }
    glPopMatrix();
}

void scene()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


    glViewport(0,0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //gluLookAt (olhoX, olhoY, olhoZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
     cam.Refresh();

    glPushMatrix();
    //glRotatef( -90.0, 1.0, 0.0, 0.0 );
   // glRotatef( rotationX, 0.0, 0.0, 1.0 );

    //if(grupos[grupoAtual].verticesGrupo1.size()>1)

   // glPushMatrix();
   // glRotatef( -180.0, 1.0, 0.0, 0.0 );
   // glRotatef(90,1.0,0.0,0.0);

   // glPopMatrix();
   // quadric = gluNewQuadric();
    glPushMatrix();
  // glTranslatef(0,0,-5);
   glTranslatef(0,-20,0);
   glPushMatrix();
   glScalef(1,0.1,1);
   glutSolidCube(60);
   glPopMatrix();
   glPopMatrix();

   glPushMatrix();

    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    glPushMatrix();
  //  glTranslatef(0,-11,0);
 //   glRotatef(-90,1.0,0.0,0.0);

    drawObject();
    glPopMatrix();
    for(int n = 0; n< objetos.size(); n++)
    {
         glPushMatrix();
            //glRotated(90, 0.0,1.0,0.0);
            //cout<<objetos[n].facesImg[0].vetorNormal.x<<endl;
          //  glTranslated(objetos[n].posX,objetos[n].alturaDescida,objetos[n].posY);
            glTranslated(objetos[n].posX,objetos[n].alturaDescida,objetos[n].posY);
            glScalef(objetos[n].escala,objetos[n].escala,objetos[n].escala);
            glRotatef(objetos[n].orientacao,0.0,1.0,0.0);
            glRotatef(objetos[n].orientacaoEmPe,1.0,0.0,0.0);
            drawObjectPly(n);
            glPopMatrix();
    }
   glPopMatrix();

}
///------------------------------------------------------------T2---------------------------------------------------------------





void display(void)
{
Timer();
    if(modoNavegacao)
        scene();
    else
        wallCreator();


    barraT->setAltura(altura);

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
     case 'm':
        if(modoNavegacao)
            modoNavegacao = false;
        else
            modoNavegacao = true;
        break;
     case 'p':
        if(modoPly)
            modoPly = false;
        else
        {

            cout<<"MODO PLY ATIVADO"<<endl;
            cout<<"Digite o angulo que deseja para seu objeto"<<endl;
            cin>>novoAng;
            cout<<"Digite qual objeto voce deseja(formiga, vaca, boneco de neve)"<<endl;
            cout<<"Para criar um formiga, digite F"<<endl;
            cout<<"Para criar uma vaca, digite V"<<endl;
            cout<<"Para criar um boneco de neve, digite B"<<endl;
            cin>>ply;
            cout<<"Agora clique na tela para decidir a localizacao do objeto"<<endl;
            modoPly = true;
        }

        break;
    }
    g_key[key] = true;
}

void specialKeysPress (int key, int x, int y)
{


    switch (tolower(key))
    {
    case GLUT_KEY_UP:
        altura++;
        olhoY-=2.5;
        break;
    case GLUT_KEY_DOWN:
        if(altura>0)
        altura--;
        olhoY+=2.5;
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

    if(modoNavegacao)
    {
        MouseMotion(x,y);
    }else
    {
        rotationX += (float) (y - last_y);
        rotationY += (float) (x - last_x);

        last_x = x;
        last_y = y;
    }

}


// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !modoNavegacao)
    {
        last_x = x;
        last_y = y;
        posZ = ((float)x*4)/width -1;
        posY = -(((float)y*2)/height -1);
        if(posZ<1)
        {
            if(!modoPly)
            {
                vertice newVertice;

                newVertice.espessura = esp;
                newVertice.x = altura;
                newVertice.y = posY*30;
                newVertice.z = posZ*30;
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
            }else
            {
            leArquivoPly(posZ*30, posY*30, novoAng, ply);
            modoPly = false;
            }

            //grupos[grupoAtual].verticesGrupo2.push_back(newVertice);
        }
        // drawNewObject();

    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && !modoNavegacao )
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
    //glutPassiveMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(specialKeysPress);
    glutIdleFunc(idle);
    desenhaEixos();
    desenho2D();
    glutMainLoop();
//    glutTimerFunc(1, Timer, 0);
    return 0;
}
