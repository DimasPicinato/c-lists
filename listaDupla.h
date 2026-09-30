#ifndef LISTADUPLA_H
#define LISTADUPLA_H

#include <limits.h>

#ifndef DIR_DADOS
#define DIR_DADOS "dados"
#endif

#define LISTAD_ASSINATURA "LISTD001"
#define LISTAD_ARQUIVO DIR_DADOS "/lista_dupla.dat"
#define LISTAD_SEM_VALOR INT_MAX

typedef struct NoDuplo {
    int valor;
    struct NoDuplo *prox;
    struct NoDuplo *ant;
} NoDuplo;

extern NoDuplo *cabecaListaDupla;

void ListaDuplaInicializar(void);
int ListaDuplaChecaLista(void);
void ListaDuplaAdicionaFim(int valor);
void ListaDuplaAdicionaInicio(int valor);
void ListaDuplaMostraLista(void);
int ListaDuplaRemoveInicio(void);
int ListaDuplaRemoveFim(void);
void ListaDuplaLimpaLista(void);
int ListaDuplaTamanhoDaLista(void);
int ListaDuplaRemovePosicao(int posicao);
void ListaDuplaAdicionarOrdenado(int valor);
int ListaDuplaGravarListaArquivo(void);
int ListaDuplaRecuperarListaArquivo(void);

#endif
