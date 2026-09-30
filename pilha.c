#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pilha.h"

NoPilha *topoPilha = NULL;

static int inicializada = 0;

static NoPilha *criarNo(int valor)
{
    NoPilha *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        puts("Erro: memoria insuficiente para alocar o no.");
        return NULL;
    }
    novo->valor = valor;
    novo->prox = NULL;
    return novo;
}

void PilhaInicializar(void)
{
    if (inicializada)
        return;
    topoPilha = NULL;
    inicializada = 1;
}

int PilhaChecaLista(void)
{
    return (topoPilha != NULL);
}

void PilhaAdiciona(int valor)
{
    NoPilha *novo = criarNo(valor);
    if (novo == NULL)
        return;
    novo->prox = topoPilha;
    topoPilha = novo;
}

int PilhaRemove(void)
{
    if (topoPilha == NULL) {
        puts("Pilha vazia: nada a remover.");
        return PILHA_SEM_VALOR;
    }
    NoPilha *removido = topoPilha;
    topoPilha = removido->prox;
    int valor = removido->valor;
    free(removido);
    return valor;
}

void PilhaMostraLista(void)
{
    if (topoPilha == NULL) {
        puts("Pilha (topo -> base): (vazia)");
        return;
    }
    printf("Pilha (topo -> base):");
    for (NoPilha *atual = topoPilha; atual != NULL; atual = atual->prox)
        printf(" %d%s", atual->valor, atual->prox != NULL ? " ->" : "");
    puts("");
}

void PilhaLimpaLista(void)
{
    NoPilha *atual = topoPilha;
    while (atual != NULL) {
        NoPilha *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    topoPilha = NULL;
}

int PilhaTamanhoDaLista(void)
{
    int total = 0;
    for (NoPilha *atual = topoPilha; atual != NULL; atual = atual->prox)
        total++;
    return total;
}

int PilhaGravarListaArquivo(void)
{
    FILE *arquivo = fopen(PILHA_ARQUIVO, "wb");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir '%s' para gravacao.\n",
               PILHA_ARQUIVO);
        return 0;
    }
    int quantidade = PilhaTamanhoDaLista();
    if (fwrite(PILHA_ASSINATURA, 1, sizeof(PILHA_ASSINATURA) - 1, arquivo) !=
            sizeof(PILHA_ASSINATURA) - 1 ||
        fwrite(&quantidade, sizeof(int), 1, arquivo) != 1) {
        puts("Erro: falha ao escrever o cabecalho do arquivo.");
        fclose(arquivo);
        return 0;
    }
    for (NoPilha *atual = topoPilha; atual != NULL; atual = atual->prox) {
        if (fwrite(&atual->valor, sizeof(int), 1, arquivo) != 1) {
            puts("Erro: falha ao escrever um elemento no arquivo.");
            fclose(arquivo);
            return 0;
        }
    }
    fclose(arquivo);
    printf("Pilha gravada em '%s' (%d elemento(s)).\n", PILHA_ARQUIVO,
           quantidade);
    return 1;
}

int PilhaRecuperarListaArquivo(void)
{
    FILE *arquivo = fopen(PILHA_ARQUIVO, "rb");
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado.\n", PILHA_ARQUIVO);
        return 0;
    }
    char assinatura[sizeof(PILHA_ASSINATURA)];
    int quantidade = 0;
    if (fread(assinatura, 1, sizeof(PILHA_ASSINATURA) - 1, arquivo) !=
        sizeof(PILHA_ASSINATURA) - 1) {
        puts("Arquivo corrompido: assinatura invalida ou cabecalho ausente.");
        fclose(arquivo);
        return 0;
    }
    assinatura[sizeof(assinatura) - 1] = '\0';
    if (strcmp(assinatura, PILHA_ASSINATURA) != 0) {
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
    PilhaLimpaLista();
    for (int i = quantidade - 1; i >= 0; i--)
        PilhaAdiciona(valores[i]);
    free(valores);
    printf("Pilha recuperada de '%s' (%d elemento(s)).\n", PILHA_ARQUIVO,
           quantidade);
    return 1;
}
