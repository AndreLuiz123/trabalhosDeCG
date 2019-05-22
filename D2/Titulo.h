#ifndef TITULO_H_INCLUDED
#define TITULO_H_INCLUDED

class Titulo{

    private:
      char suf[64];
      char pref[64];
      int numFaces;


    public:
      Titulo();
      Titulo(char *suf, char *nomeObjeto, int numeroFaces);
      void setSufixo(char *suf);
      void setPrefixo(char *pref);
      void setAltura(int alt);
      void alteraTitulo();
};

#endif // TITULO_H_INCLUDED
