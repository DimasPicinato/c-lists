#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fila.h"

NoFila *traseiraFila = NULL;

static int inicializada = 0;

static NoFila *criarNo(int valor)
{
    NoFila *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        puts("Erro: memoria insuficiente para alocar o no.");
        return NULL;
    }
    novo->valor = valor;
    novo->prox = NULL;
    return novo;
}

static NoFila *frente(void)
{
    if (traseiraFila == NULL)
        return NULL;
    return traseiraFila->prox;
}

static int filaVazia(void)
{
    return (traseiraFila == NULL);
}

void FilaInicializar(void)
{
    if (inicializada)
        return;
    traseiraFila = NULL;
    inicializada = 1;
}

int FilaChecaLista(void)
{
    return !filaVazia();
}

void FilaAdiciona(int valor)
{
    NoFila *novo = criarNo(valor);
    if (novo == NULL)
        return;
    if (filaVazia()) {
        novo->prox = novo;
        traseiraFila = novo;
        return;
    }
    novo->prox = traseiraFila->prox;
    traseiraFila->prox = novo;
    traseiraFila = novo;
}

int FilaRemove(void)
{
    if (filaVazia()) {
        puts("Fila vazia: nada a remover.");
        return FILA_SEM_VALOR;
    }
    NoFila *primeiro = frente();
    int valor = primeiro->valor;
    if (primeiro == traseiraFila) {
        free(primeiro);
        traseiraFila = NULL;
        return valor;
    }
    traseiraFila->prox = primeiro->prox;
    free(primeiro);
    return valor;
}

void FilaMostraLista(void)
{
    if (filaVazia()) {
        puts("Fila (frente -> traseira): (vazia)");
        return;
    }
    printf("Fila (frente -> traseira):");
    for (NoFila *atual = frente();; atual = atual->prox) {
        printf(" %d", atual->valor);
        if (atual == traseiraFila)
            break;
        printf(" ->");
    }
    puts("");
}

void FilaLimpaLista(void)
{
    if (filaVazia())
        return;
    NoFila *atual = frente();
    while (atual != traseiraFila) {
        NoFila *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    free(traseiraFila);
    traseiraFila = NULL;
}

int FilaTamanhoDaLista(void)
{
    if (filaVazia())
        return 0;
    int total = 0;
    for (NoFila *atual = frente();; atual = atual->prox) {
        total++;
        if (atual == traseiraFila)
            break;
    }
    return total;
}

int FilaGravarListaArquivo(void)
{
    FILE *arquivo = fopen(FILA_ARQUIVO, "wb");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir '%s' para gravacao.\n",
               FILA_ARQUIVO);
        return 0;
    }
    int quantidade = FilaTamanhoDaLista();
    if (fwrite(FILA_ASSINATURA, 1, sizeof(FILA_ASSINATURA) - 1, arquivo) !=
            sizeof(FILA_ASSINATURA) - 1 ||
        fwrite(&quantidade, sizeof(int), 1, arquivo) != 1) {
        puts("Erro: falha ao escrever o cabecalho do arquivo.");
        fclose(arquivo);
        return 0;
    }
    if (!filaVazia()) {
        for (NoFila *atual = frente();; atual = atual->prox) {
            if (fwrite(&atual->valor, sizeof(int), 1, arquivo) != 1) {
                puts("Erro: falha ao escrever um elemento no arquivo.");
                fclose(arquivo);
                return 0;
            }
            if (atual == traseiraFila)
                break;
        }
    }
    fclose(arquivo);
    printf("Fila gravada em '%s' (%d elemento(s)).\n", FILA_ARQUIVO, quantidade);
    return 1;
}

int FilaRecuperarListaArquivo(void)
{
    FILE *arquivo = fopen(FILA_ARQUIVO, "rb");
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado.\n", FILA_ARQUIVO);
        return 0;
    }
    char assinatura[sizeof(FILA_ASSINATURA)];
    int quantidade = 0;
    if (fread(assinatura, 1, sizeof(FILA_ASSINATURA) - 1, arquivo) !=
        sizeof(FILA_ASSINATURA) - 1) {
        puts("Arquivo corrompido: assinatura invalida ou cabecalho ausente.");
        fclose(arquivo);
        return 0;
    }
    assinatura[sizeof(assinatura) - 1] = '\0';
    if (strcmp(assinatura, FILA_ASSINATURA) != 0) {
        puts("Arquivo corrompido: assinatura invalida ou cabecalho ausente.");
        fclose(arquivo);
        return 0;
    }
    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 || quantidade < 0) {
        puts("Arquivo corrompido: quantidade de elementos invalida.");
        fclose(arquivo);
        return 0;
    }
    int *valores = NULL;
    if (quantidade > 0) {
        valores = malloc((size_t) quantidade * sizeof *valores);
        if (valores == NULL) {
            puts("Erro: memoria insuficiente para ler o arquivo.");
            fclose(arquivo);
            return 0;
        }
    }
    for (int i = 0; i < quantidade; i++) {
        if (fread(&valores[i], sizeof(int), 1, arquivo) != 1) {
            printf("Arquivo corrompido: esperado %d elemento(s), gravacao "
                   "interrompida no %d.\n",
                   quantidade, i);
            free(valores);
            fclose(arquivo);
            return 0;
        }
    }
    fclose(arquivo);
    FilaLimpaLista();
    for (int i = 0; i < quantidade; i++)
        FilaAdiciona(valores[i]);
    free(valores);
    printf("Fila recuperada de '%s' (%d elemento(s)).\n", FILA_ARQUIVO,
           quantidade);
    return 1;
}
