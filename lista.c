#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"

No *cabecaLista = NULL;

static int inicializada = 0;

static No *criarNo(int valor)
{
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        puts("Erro: memoria insuficiente para alocar o no.");
        return NULL;
    }
    novo->valor = valor;
    novo->prox = NULL;
    return novo;
}

void ListaInicializar(void)
{
    if (inicializada)
        return;
    cabecaLista = NULL;
    inicializada = 1;
}

int ListaChecaLista(void)
{
    return (cabecaLista != NULL);
}

void ListaAdicionaFim(int valor)
{
    No *novo = criarNo(valor);
    if (novo == NULL)
        return;
    if (cabecaLista == NULL) {
        cabecaLista = novo;
        return;
    }
    No *atual = cabecaLista;
    while (atual->prox != NULL)
        atual = atual->prox;
    atual->prox = novo;
}

void ListaAdicionaInicio(int valor)
{
    No *novo = criarNo(valor);
    if (novo == NULL)
        return;
    novo->prox = cabecaLista;
    cabecaLista = novo;
}

void ListaMostraLista(void)
{
    if (cabecaLista == NULL) {
        puts("Lista encadeada: (vazia)");
        return;
    }
    printf("Lista encadeada:");
    for (No *atual = cabecaLista; atual != NULL; atual = atual->prox)
        printf(" %d%s", atual->valor, atual->prox != NULL ? " ->" : "");
    puts("");
}

int ListaRemoveInicio(void)
{
    if (cabecaLista == NULL) {
        puts("Lista vazia: nada a remover do inicio.");
        return LISTA_SEM_VALOR;
    }
    No *removido = cabecaLista;
    cabecaLista = removido->prox;
    int valor = removido->valor;
    free(removido);
    return valor;
}

int ListaRemoveFim(void)
{
    if (cabecaLista == NULL) {
        puts("Lista vazia: nada a remover do fim.");
        return LISTA_SEM_VALOR;
    }
    if (cabecaLista->prox == NULL) {
        int valor = cabecaLista->valor;
        free(cabecaLista);
        cabecaLista = NULL;
        return valor;
    }
    No *atual = cabecaLista;
    while (atual->prox->prox != NULL)
        atual = atual->prox;
    int valor = atual->prox->valor;
    free(atual->prox);
    atual->prox = NULL;
    return valor;
}

void ListaLimpaLista(void)
{
    No *atual = cabecaLista;
    while (atual != NULL) {
        No *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    cabecaLista = NULL;
}

int ListaTamanhoDaLista(void)
{
    int total = 0;
    for (No *atual = cabecaLista; atual != NULL; atual = atual->prox)
        total++;
    return total;
}

int ListaRemovePosicao(int posicao)
{
    if (posicao < 0) {
        printf("Posicao invalida: %d (a base e 0).\n", posicao);
        return LISTA_SEM_VALOR;
    }
    if (cabecaLista == NULL) {
        puts("Lista vazia: nada a remover.");
        return LISTA_SEM_VALOR;
    }
    No *anterior = NULL;
    No *atual = cabecaLista;
    int indice = 0;
    while (atual != NULL && indice < posicao) {
        anterior = atual;
        atual = atual->prox;
        indice++;
    }
    if (atual == NULL) {
        printf("Posicao %d fora do intervalo [0, %d].\n", posicao,
               ListaTamanhoDaLista() - 1);
        return LISTA_SEM_VALOR;
    }
    if (anterior != NULL)
        anterior->prox = atual->prox;
    else
        cabecaLista = atual->prox;
    int valor = atual->valor;
    free(atual);
    return valor;
}

void ListaAdicionarOrdenado(int valor)
{
    No *novo = criarNo(valor);
    if (novo == NULL)
        return;
    if (cabecaLista == NULL || valor < cabecaLista->valor) {
        novo->prox = cabecaLista;
        cabecaLista = novo;
        return;
    }
    No *atual = cabecaLista;
    while (atual->prox != NULL && atual->prox->valor <= valor)
        atual = atual->prox;
    novo->prox = atual->prox;
    atual->prox = novo;
}

int ListaGravarListaArquivo(void)
{
    FILE *arquivo = fopen(LISTA_ARQUIVO, "wb");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir '%s' para gravacao.\n",
               LISTA_ARQUIVO);
        return 0;
    }
    int quantidade = ListaTamanhoDaLista();
    if (fwrite(LISTA_ASSINATURA, 1, sizeof(LISTA_ASSINATURA) - 1, arquivo) !=
            sizeof(LISTA_ASSINATURA) - 1 ||
        fwrite(&quantidade, sizeof(int), 1, arquivo) != 1) {
        puts("Erro: falha ao escrever o cabecalho do arquivo.");
        fclose(arquivo);
        return 0;
    }
    for (No *atual = cabecaLista; atual != NULL; atual = atual->prox) {
        if (fwrite(&atual->valor, sizeof(int), 1, arquivo) != 1) {
            puts("Erro: falha ao escrever um elemento no arquivo.");
            fclose(arquivo);
            return 0;
        }
    }
    fclose(arquivo);
    printf("Lista gravada em '%s' (%d elemento(s)).\n", LISTA_ARQUIVO,
           quantidade);
    return 1;
}

int ListaRecuperarListaArquivo(void)
{
    FILE *arquivo = fopen(LISTA_ARQUIVO, "rb");
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado.\n", LISTA_ARQUIVO);
        return 0;
    }
    char assinatura[sizeof(LISTA_ASSINATURA)];
    int quantidade = 0;
    if (fread(assinatura, 1, sizeof(LISTA_ASSINATURA) - 1, arquivo) !=
        sizeof(LISTA_ASSINATURA) - 1) {
        puts("Arquivo corrompido: assinatura invalida ou cabecalho ausente.");
        fclose(arquivo);
        return 0;
    }
    assinatura[sizeof(assinatura) - 1] = '\0';
    if (strcmp(assinatura, LISTA_ASSINATURA) != 0) {
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
    ListaLimpaLista();
    for (int i = 0; i < quantidade; i++)
        ListaAdicionaFim(valores[i]);
    free(valores);
    printf("Lista recuperada de '%s' (%d elemento(s)).\n", LISTA_ARQUIVO,
           quantidade);
    return 1;
}
