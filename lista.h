#ifndef LISTA_H
#define LISTA_H

#include <limits.h>

#ifndef DIR_DADOS
#define DIR_DADOS "dados"
#endif

#define LISTA_ASSINATURA "LISTA001"
#define LISTA_ARQUIVO DIR_DADOS "/lista_encadeada.dat"
#define LISTA_SEM_VALOR INT_MAX

typedef struct No {
    int valor;
    struct No *prox;
} No;

extern No *cabecaLista;

void ListaInicializar(void);
int ListaChecaLista(void);
void ListaAdicionaFim(int valor);
void ListaAdicionaInicio(int valor);
void ListaMostraLista(void);
int ListaRemoveInicio(void);
int ListaRemoveFim(void);
void ListaLimpaLista(void);
int ListaTamanhoDaLista(void);
int ListaRemovePosicao(int posicao);
void ListaAdicionarOrdenado(int valor);
int ListaGravarListaArquivo(void);
int ListaRecuperarListaArquivo(void);

#endif
