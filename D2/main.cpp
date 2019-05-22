#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include "extras.h"
#include <vector>
#include "Titulo.h"
/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.
class vertice
{
public:
    double x,y,z;
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
    float zdist;
    std::vector<face> facesImg;
    float material[13];
    int nTriang=0;
};

class triangle
{
public:
    vertice v[3];
};

/// Globals
float zdist = 5.0;
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width, height;
float teste=0;
int indiceObj=0;
std::ifstream arquivoPly;
std::string str;
int numeroVertices;
int numeroFaces;
int numeroPropriedades = -1;
std::vector<vertice> verticesImg;
std::vector<face> facesImg;
bool wireframe = false;
bool telaCheia = false;
float maiorY=-1000;
float menorY=10000;
float maiorX=-1000;
float menorX=1000;
float maiorZ=-1000;

std::vector<objeto> objetos;
std::ifstream budda;
std::ifstream coelho;
std::ifstream vaca;
std::ifstream dragao;
std::ifstream dragaoCompleto;
std::ifstream bonecoNeve;

float Brass[] = {0.329412, 0.223529, 0.027451, 1.000000,0.780392, 0.568627, 0.113725, 1.000000,0.992157, 0.941176, 0.807843, 1.000000,27.897400};
float Bronze[] = {0.212500, 0.127500, 0.054000, 1.000000,0.714000, 0.428400, 0.181440, 1.000000,0.393548, 0.271906, 0.166721, 1.000000,25.600000};
float Emerald[] = {0.021500, 0.174500, 0.021500, 0.550000,0.075680, 0.614240, 0.075680, 0.550000,0.633000, 0.727811, 0.633000, 0.550000,76.800003};
float Copper[] = {0.191250, 0.073500, 0.022500, 1.000000,0.703800, 0.270480, 0.082800, 1.000000,0.256777, 0.137622, 0.086014, 1.000000,12.800000};
float Pearl[] = {0.250000, 0.207250, 0.207250, 0.922000,1.000000, 0.829000, 0.829000, 0.922000,0.296648, 0.296648, 0.296648, 0.922000,11.264000};
float Ruby[] = {0.174500, 0.011750, 0.011750, 0.550000,0.614240, 0.041360, 0.041360, 0.550000,0.727811, 0.626959, 0.626959, 0.550000,76.800003};
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

Titulo *title = new Titulo("Numero de triangulos: ", "---", numeroFaces);

void CalculaNormal(face t, vertice *vn)
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

void leArquivoPly(std::ifstream& arquivo)
{

    objeto obj;
    bool leArquivo=true;

    while(leArquivo)
    {
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
        CalculaNormal(f, &f.vetorNormal);

        facesImg.push_back(f);
    }


    obj.posY = maiorY - abs(menorY-maiorY) ;
    obj.posX = maiorX - abs(menorX-maiorX)/2;
    obj.zdist = abs(maiorY - obj.posY)/tan(3.14/9);
    obj.facesImg = facesImg;
    facesImg.clear();
    verticesImg.clear();
    objetos.push_back(obj);
    numeroPropriedades = -1;
    menorY = 0;
    maiorY = 0;
    maiorZ = 0;
}

void novoMaterial()
{
    switch(indiceObj)
    {
        case 0:
            setMaterials(Brass);
            break;
        case 1:
            setMaterials(Bronze);
            break;
        case 2:
            setMaterials(Emerald);
            break;
        case 3:
            setMaterials(Copper);
            break;
        case 4:
            setMaterials(Pearl);
            break;
        case 5:
            setMaterials(Ruby);
            break;
    }
}

void novoTitulo()
{
    switch(indiceObj)
    {
        case 0:
            title->setPrefixo("cow.ply");
            title->setAltura(objetos[0].nTriang);
            title->alteraTitulo();
            break;
        case 1:
            title->setPrefixo("budda.ply");
            title->setAltura(objetos[1].nTriang);
            title->alteraTitulo();
            break;
        case 2:
            title->setPrefixo("bunny.ply");
            title->setAltura(objetos[2].nTriang);
            title->alteraTitulo();
            break;
        case 3:
            title->setPrefixo("dragon.ply");
            title->setAltura(objetos[3].nTriang);
            title->alteraTitulo();
            break;
        case 4:
            title->setPrefixo("dragon_full.ply");
            title->setAltura(objetos[4].nTriang);
            title->alteraTitulo();
            break;
        case 5:
            title->setPrefixo("snowman.ply");
            title->setAltura(objetos[5].nTriang);
            title->alteraTitulo();
            break;
    }
}


/// Functions
void init(void)
{
    title->setPrefixo("Calculando...");
    title->setAltura(0);
    title->alteraTitulo();
    initLight(width, height); // Função extra para tratar iluminação.
    glEnable(GL_DEPTH_TEST);
    vaca.open("cow.ply");
    budda.open("budda.ply");
    coelho.open("bunny.ply");
    dragao.open("dragon.ply");
    dragaoCompleto.open("dragon_full.ply");
    bonecoNeve.open("snowman.ply");
    leArquivoPly(vaca);
    leArquivoPly(budda);
    leArquivoPly(coelho);
    leArquivoPly(dragao);
    leArquivoPly(dragaoCompleto);
    leArquivoPly(bonecoNeve);

    novoMaterial();

    std::cout<<"Para ativar/desativar o modo wireframe, pressione a tecla 'A'"<<std::endl;
    std::cout<<"Para visualizar outros objetos, pressione as teclas da esquerda e direita"<<std::endl;
    std::cout<<"Para ativar/desativar a tela cheia, pressione f12"<<std::endl;
    std::cout<<"Para fechar a aplicacao, pressione esc"<<std::endl;

    novoTitulo();

}


void drawObject()
{

    vertice vetorNormal;

    for(int i = 0; i < objetos[indiceObj].facesImg.size(); i++)
    {
        //CalculaNormal(facesImg[i], &vetorNormal);
        glNormal3f(objetos[indiceObj].facesImg[i].vetorNormal.x, objetos[indiceObj].facesImg[i].vetorNormal.y,objetos[indiceObj].facesImg[i].vetorNormal.z);
        if(!wireframe){
        glBegin(GL_POLYGON);
        for(int j=0; j < objetos[indiceObj].facesImg[i].nVertices; j++)
        {

            glVertex3d(objetos[indiceObj].facesImg[i].verticesFace[j].x,objetos[indiceObj].facesImg[i].verticesFace[j].y,objetos[indiceObj].facesImg[i].verticesFace[j].z);
        }
        glEnd();
        }else{

        glBegin(GL_LINE_STRIP);
        for(int j=0; j < objetos[indiceObj].facesImg[i].nVertices; j++)
        {

            glVertex3d(objetos[indiceObj].facesImg[i].verticesFace[j].x,objetos[indiceObj].facesImg[i].verticesFace[j].y,objetos[indiceObj].facesImg[i].verticesFace[j].z);
        }
        glEnd();

        }

    }
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt (objetos[indiceObj].posX, objetos[indiceObj].posY, objetos[indiceObj].zdist,0.0,0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();
    glRotatef( rotationY, 0.0, 1.0, 0.0 );
    glRotatef( rotationX, 1.0, 0.0, 0.0 );
    drawObject();
    glPopMatrix();

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

    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.01, 200.0);
}

void keyboard (unsigned char key, int x, int y)
{

    switch (tolower(key))
    {

    case 'a':
        if(wireframe)
        wireframe = false;
        else
        wireframe = true;
        break;
    case 27:
        exit(0);
        break;
    }
}

void specialKeysPress(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_RIGHT:
            if(indiceObj<5)
            indiceObj++;
            novoMaterial();
            novoTitulo();
            break;
        case GLUT_KEY_LEFT:
            if(indiceObj>0)
            indiceObj--;
            novoMaterial();
            novoTitulo();
            break;
        case GLUT_KEY_F12:
            if(!telaCheia)
            {
                glutFullScreen();
                telaCheia = true;
            }else
            {
                glutReshapeWindow(800,600);
                telaCheia = false;
            }
            break;

        default:
            break;
    }
    glutPostRedisplay();
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
    }
    if(button == 3) // Scroll up
    {
        objetos[indiceObj].zdist+=0.1f;
    }
    if(button == 4) // Scroll Down
    {
        objetos[indiceObj].zdist-=0.1f;
    }
}

/// Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
     glutSpecialFunc(specialKeysPress);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
