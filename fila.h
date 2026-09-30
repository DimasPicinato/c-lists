#ifndef FILA_H
#define FILA_H

#include <limits.h>

#ifndef DIR_DADOS
#define DIR_DADOS "dados"
#endif

#define FILA_ASSINATURA "FILA0001"
#define FILA_ARQUIVO DIR_DADOS "/fila.dat"
#define FILA_SEM_VALOR INT_MAX

typedef struct NoFila {
    int valor;
    struct NoFila *prox;
} NoFila;

extern NoFila *traseiraFila;

void FilaInicializar(void);
int FilaChecaLista(void);
void FilaAdiciona(int valor);
int FilaRemove(void);
void FilaMostraLista(void);
void FilaLimpaLista(void);
int FilaTamanhoDaLista(void);
int FilaGravarListaArquivo(void);
int FilaRecuperarListaArquivo(void);

#endif
