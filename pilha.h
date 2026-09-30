#ifndef PILHA_H
#define PILHA_H

#include <limits.h>

#ifndef DIR_DADOS
#define DIR_DADOS "dados"
#endif

#define PILHA_ASSINATURA "PILH0001"
#define PILHA_ARQUIVO DIR_DADOS "/pilha.dat"
#define PILHA_SEM_VALOR INT_MAX

typedef struct NoPilha {
    int valor;
    struct NoPilha *prox;
} NoPilha;

extern NoPilha *topoPilha;

void PilhaInicializar(void);
int PilhaChecaLista(void);
void PilhaAdiciona(int valor);
int PilhaRemove(void);
void PilhaMostraLista(void);
void PilhaLimpaLista(void);
int PilhaTamanhoDaLista(void);
int PilhaGravarListaArquivo(void);
int PilhaRecuperarListaArquivo(void);

#endif
