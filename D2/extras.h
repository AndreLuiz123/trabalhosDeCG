
void initLight(int width, int height)
{
    glEnable(GL_LIGHTING);                 // Habilita luz
    glEnable(GL_LIGHT0);                   // habilita luz 0

    // Cor da fonte de luz (RGBA)
    GLfloat cor_luz[]     = { 1.0, 1.0, 1.0, 1.0};
    // Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
    GLfloat posicao_luz[] = { (float) width, (float)height, 1000.0, 1.0};

    // Define parametros da luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    // Quando a op��o "two_side lighting" est� ativada, o opengl inverte a orienta��o das normais
    // permitindo que tanto as faces externas quanto internas sejam iluminadas.
    // Essa op��o � mais lenta se comparada a renderiza��o de apenas um dos lados
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


void setMaterials(float vetor[])
{
   // Parametros comuns para os dois lados da superf�cie
        GLfloat objeto_especular[] = { vetor[8], vetor[9], vetor[10], vetor[11] };
        GLfloat objeto_brilho[]    = { vetor[12] };

        // Material da superf�cie externa
        GLfloat objeto_ambient[]   = {vetor[0], vetor[1], vetor[2], vetor[3]};
        GLfloat objeto_difusa[]    = {vetor[4], vetor[5], vetor[6], vetor[7]};

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        // Material das faces internas (amarelo)
        GLfloat objeto_ambient_back[]   = { 0.1745, 0.174, 0.011, 1.0 };
        GLfloat objeto_difusa_back[]    = { 0.6144, 0.614, 0.041, 1.0 };

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient_back);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa_back);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);

}

/*float Brass[] = {0.329412, 0.223529, 0.027451, 1.000000,0.780392, 0.568627, 0.113725, 1.000000,0.992157, 0.941176, 0.807843, 1.000000,27.897400};
float Bronze[] = {0.212500, 0.127500, 0.054000, 1.000000,0.714000, 0.428400, 0.181440, 1.000000,0.393548, 0.271906, 0.166721, 1.000000,25.600000};
float Polished_Bronze[] = {0.250000, 0.148000, 0.064750, 1.000000,0.400000, 0.236800, 0.103600, 1.000000,0.774597, 0.458561, 0.200621, 1.000000,76.800003};
float Chrome[] = {0.250000, 0.250000, 0.250000, 1.000000,0.400000, 0.400000, 0.400000, 1.000000,0.774597, 0.774597, 0.774597, 1.000000,76.800003};
float Copper[] = {0.191250, 0.073500, 0.022500, 1.000000,0.703800, 0.270480, 0.082800, 1.000000,0.256777, 0.137622, 0.086014, 1.000000,12.800000};
float Polished_Copper[] = {0.229500, 0.088250, 0.027500, 1.000000,0.550800, 0.211800, 0.066000, 1.000000,0.580594, 0.223257, 0.069570, 1.000000,51.200001};
float Gold[] = {0.247250, 0.199500, 0.074500, 1.000000,0.751640, 0.606480, 0.226480, 1.000000,0.628281, 0.555802, 0.366065, 1.000000,51.200001};
float Polished_Gold[] = {0.247250, 0.224500, 0.064500, 1.000000,0.346150, 0.314300, 0.090300, 1.000000,0.797357, 0.723991, 0.208006, 1.000000,83.199997};
float Pewter[] = {0.105882, 0.058824, 0.113725, 1.000000,0.427451, 0.470588, 0.541176, 1.000000,0.333333, 0.333333, 0.521569, 1.000000,9.846150};
float Silver[] = {0.192250, 0.192250, 0.192250, 1.000000,0.507540, 0.507540, 0.507540, 1.000000,0.508273, 0.508273, 0.508273, 1.000000,51.200001};
float Polished_Silver[] = {0.231250, 0.231250, 0.231250, 1.000000,0.277500, 0.277500, 0.277500, 1.000000,0.773911, 0.773911, 0.773911, 1.000000,89.599998};
float Emerald[] = {0.021500, 0.174500, 0.021500, 0.550000,0.075680, 0.614240, 0.075680, 0.550000,0.633000, 0.727811, 0.633000, 0.550000,76.800003};
float Jade[] = {0.135000, 0.222500, 0.157500, 0.950000,0.540000, 0.890000, 0.630000, 0.950000,0.316228, 0.316228, 0.316228, 0.950000,12.800000};
float Obsidian[] = {0.053750, 0.050000, 0.066250, 0.820000,0.182750, 0.170000, 0.225250, 0.820000,0.332741, 0.328634, 0.346435, 0.820000,38.400002};
float Pearl[] = {0.250000, 0.207250, 0.207250, 0.922000,1.000000, 0.829000, 0.829000, 0.922000,0.296648, 0.296648, 0.296648, 0.922000,11.264000};
float Ruby[] = {0.174500, 0.011750, 0.011750, 0.550000,0.614240, 0.041360, 0.041360, 0.550000,0.727811, 0.626959, 0.626959, 0.550000,76.800003};
float Turquoise[] = {0.100000, 0.187250, 0.174500, 0.800000,0.396000, 0.741510, 0.691020, 0.800000,0.297254, 0.308290, 0.306678, 0.800000,12.800000};
float Black_Plastic[] = {0.000000, 0.000000, 0.000000, 1.000000,0.010000, 0.010000, 0.010000, 1.000000,0.500000, 0.500000, 0.500000, 1.000000,32.000000};
float Black_Rubber[] = {0.020000, 0.020000, 0.020000, 1.000000,0.010000, 0.010000, 0.010000, 1.000000,0.400000, 0.400000, 0.400000, 1.000000,10.000000};
*/
