#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listaDupla.h"

NoDuplo *cabecaListaDupla = NULL;

static int inicializada = 0;

static NoDuplo *criarNo(int valor)
{
    NoDuplo *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        puts("Erro: memoria insuficiente para alocar o no.");
        return NULL;
    }
    novo->valor = valor;
    novo->prox = NULL;
    novo->ant = NULL;
    return novo;
}

void ListaDuplaInicializar(void)
{
    if (inicializada)
        return;
    cabecaListaDupla = NULL;
    inicializada = 1;
}

int ListaDuplaChecaLista(void)
{
    return (cabecaListaDupla != NULL);
}

void ListaDuplaAdicionaFim(int valor)
{
    NoDuplo *novo = criarNo(valor);
    if (novo == NULL)
        return;
    if (cabecaListaDupla == NULL) {
        cabecaListaDupla = novo;
        return;
    }
    NoDuplo *atual = cabecaListaDupla;
    while (atual->prox != NULL)
        atual = atual->prox;
    atual->prox = novo;
    novo->ant = atual;
}

void ListaDuplaAdicionaInicio(int valor)
{
    NoDuplo *novo = criarNo(valor);
    if (novo == NULL)
        return;
    novo->prox = cabecaListaDupla;
    if (cabecaListaDupla != NULL)
        cabecaListaDupla->ant = novo;
    cabecaListaDupla = novo;
}

void ListaDuplaMostraLista(void)
{
    if (cabecaListaDupla == NULL) {
        puts("Lista duplamente encadeada: (vazia)");
        return;
    }
    printf("Lista duplamente encadeada:");
    for (NoDuplo *atual = cabecaListaDupla; atual != NULL; atual = atual->prox)
        printf(" %d%s", atual->valor, atual->prox != NULL ? " <->" : "");
    puts("");
}

int ListaDuplaRemoveInicio(void)
{
    if (cabecaListaDupla == NULL) {
        puts("Lista vazia: nada a remover do inicio.");
        return LISTAD_SEM_VALOR;
    }
    NoDuplo *removido = cabecaListaDupla;
    cabecaListaDupla = removido->prox;
    if (cabecaListaDupla != NULL)
        cabecaListaDupla->ant = NULL;
    int valor = removido->valor;
    free(removido);
    return valor;
}

int ListaDuplaRemoveFim(void)
{
    if (cabecaListaDupla == NULL) {
        puts("Lista vazia: nada a remover do fim.");
        return LISTAD_SEM_VALOR;
    }
    if (cabecaListaDupla->prox == NULL) {
        int valor = cabecaListaDupla->valor;
        free(cabecaListaDupla);
        cabecaListaDupla = NULL;
        return valor;
    }
    NoDuplo *atual = cabecaListaDupla;
    while (atual->prox->prox != NULL)
        atual = atual->prox;
    int valor = atual->prox->valor;
    free(atual->prox);
    atual->prox = NULL;
    return valor;
}

void ListaDuplaLimpaLista(void)
{
    NoDuplo *atual = cabecaListaDupla;
    while (atual != NULL) {
        NoDuplo *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    cabecaListaDupla = NULL;
}

int ListaDuplaTamanhoDaLista(void)
{
    int total = 0;
    for (NoDuplo *atual = cabecaListaDupla; atual != NULL; atual = atual->prox)
        total++;
    return total;
}

int ListaDuplaRemovePosicao(int posicao)
{
    if (posicao < 0) {
        printf("Posicao invalida: %d (a base e 0).\n", posicao);
        return LISTAD_SEM_VALOR;
    }
    if (cabecaListaDupla == NULL) {
        puts("Lista vazia: nada a remover.");
        return LISTAD_SEM_VALOR;
    }
    NoDuplo *anterior = NULL;
    NoDuplo *atual = cabecaListaDupla;
    int indice = 0;
    while (atual != NULL && indice < posicao) {
        anterior = atual;
        atual = atual->prox;
        indice++;
    }
    if (atual == NULL) {
        printf("Posicao %d fora do intervalo [0, %d].\n", posicao,
               ListaDuplaTamanhoDaLista() - 1);
        return LISTAD_SEM_VALOR;
    }
    if (anterior != NULL)
        anterior->prox = atual->prox;
    else
        cabecaListaDupla = atual->prox;
    if (atual->prox != NULL)
        atual->prox->ant = anterior;
    int valor = atual->valor;
    free(atual);
    return valor;
}

void ListaDuplaAdicionarOrdenado(int valor)
{
    NoDuplo *novo = criarNo(valor);
    if (novo == NULL)
        return;
    if (cabecaListaDupla == NULL || valor < cabecaListaDupla->valor) {
        novo->prox = cabecaListaDupla;
        if (cabecaListaDupla != NULL)
            cabecaListaDupla->ant = novo;
        cabecaListaDupla = novo;
        return;
    }
    NoDuplo *atual = cabecaListaDupla;
    while (atual->prox != NULL && atual->prox->valor <= valor)
        atual = atual->prox;
    novo->prox = atual->prox;
    novo->ant = atual;
    if (atual->prox != NULL)
        atual->prox->ant = novo;
    atual->prox = novo;
}

int ListaDuplaGravarListaArquivo(void)
{
    FILE *arquivo = fopen(LISTAD_ARQUIVO, "wb");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir '%s' para gravacao.\n",
               LISTAD_ARQUIVO);
        return 0;
    }
    int quantidade = ListaDuplaTamanhoDaLista();
    if (fwrite(LISTAD_ASSINATURA, 1, sizeof(LISTAD_ASSINATURA) - 1, arquivo) !=
            sizeof(LISTAD_ASSINATURA) - 1 ||
        fwrite(&quantidade, sizeof(int), 1, arquivo) != 1) {
        puts("Erro: falha ao escrever o cabecalho do arquivo.");
        fclose(arquivo);
        return 0;
    }
    for (NoDuplo *atual = cabecaListaDupla; atual != NULL; atual = atual->prox) {
        if (fwrite(&atual->valor, sizeof(int), 1, arquivo) != 1) {
            puts("Erro: falha ao escrever um elemento no arquivo.");
            fclose(arquivo);
            return 0;
        }
    }
    fclose(arquivo);
    printf("Lista dupla gravada em '%s' (%d elemento(s)).\n", LISTAD_ARQUIVO,
           quantidade);
    return 1;
}

int ListaDuplaRecuperarListaArquivo(void)
{
    FILE *arquivo = fopen(LISTAD_ARQUIVO, "rb");
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado.\n", LISTAD_ARQUIVO);
        return 0;
    }
    char assinatura[sizeof(LISTAD_ASSINATURA)];
    int quantidade = 0;
    if (fread(assinatura, 1, sizeof(LISTAD_ASSINATURA) - 1, arquivo) !=
        sizeof(LISTAD_ASSINATURA) - 1) {
        puts("Arquivo corrompido: assinatura invalida ou cabecalho ausente.");
        fclose(arquivo);
        return 0;
    }
    assinatura[sizeof(assinatura) - 1] = '\0';
    if (strcmp(assinatura, LISTAD_ASSINATURA) != 0) {
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
    ListaDuplaLimpaLista();
    for (int i = 0; i < quantidade; i++)
        ListaDuplaAdicionaFim(valores[i]);
    free(valores);
    printf("Lista dupla recuperada de '%s' (%d elemento(s)).\n", LISTAD_ARQUIVO,
           quantidade);
    return 1;
}
