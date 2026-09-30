#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "fila.h"
#include "lista.h"
#include "listaDupla.h"
#include "pilha.h"

static void limparTela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pausar(void)
{
    printf("\nPressione ENTER para continuar...");
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

static int lerInteiro(const char *mensagem)
{
    char linha[256];
    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof linha, stdin) == NULL) {
            puts("\nFim da entrada padrao. Encerrando.");
            exit(0);
        }
        char *fim;
        long convertido = strtol(linha, &fim, 10);
        while (*fim == ' ' || *fim == '\t' || *fim == '\n' || *fim == '\r')
            fim++;
        if (fim == linha || *fim != '\0' || convertido < INT_MIN ||
            convertido > INT_MAX) {
            puts("Entrada invalida. Digite um numero inteiro.");
        } else {
            return (int) convertido;
        }
    }
}

static int confirmar(const char *pergunta)
{
    char linha[64];
    printf("%s (s/N): ", pergunta);
    if (fgets(linha, sizeof linha, stdin) == NULL)
        return 0;
    return (linha[0] == 's' || linha[0] == 'S');
}

static int arquivoExiste(const char *caminho)
{
    FILE *arquivo = fopen(caminho, "rb");
    if (arquivo == NULL)
        return 0;
    fclose(arquivo);
    return 1;
}

static void prepararDiretorio(void)
{
#ifdef _WIN32
    _mkdir(DIR_DADOS);
#else
    mkdir(DIR_DADOS, 0777);
#endif
}

static void cabecalho(const char *titulo, int tamanho, const char *caminho)
{
    puts("==============================================================");
    printf(" Estrutura: %s\n", titulo);
    printf(" Tamanho:   %d elemento(s)\n", tamanho);
    printf(" Arquivo:   %s\n", caminho);
    printf(" Status:    %s\n",
           arquivoExiste(caminho) ? "disponivel para recuperacao"
                                  : "inexistente (use GravarListaArquivo)");
    puts("--------------------------------------------------------------");
}

static void novoArquivoLimpo(const char *caminho, void (*limpar)(void),
                             int (*gravar)(void))
{
    puts("--------------------------------------------------------------");
    puts("ATENCAO: esta opcao esvazia a estrutura na memoria e sobrescreve");
    puts("o arquivo do disco. O conteudo atual sera perdido.");
    printf("Arquivo que sera criado: %s\n", caminho);
    if (!confirmar("Deseja continuar")) {
        puts("Operacao cancelada.");
        return;
    }
    limpar();
    gravar();
    puts("Novo arquivo limpo criado.");
}

static void menuLista(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("LISTA ENCADEADA", ListaTamanhoDaLista(), LISTA_ARQUIVO);
        ListaMostraLista();
        puts("--------------------------------------------------------------");
        puts("  1 - Inicializar");
        puts("  2 - ChecaLista");
        puts("  3 - AdicionaFim");
        puts("  4 - AdicionaInicio");
        puts("  5 - MostraLista");
        puts("  6 - RemoveInicio");
        puts("  7 - RemoveFim");
        puts("  8 - LimpaLista");
        puts("  9 - TamanhoDaLista");
        puts(" 10 - RemovePosicao");
        puts(" 11 - AdicionarOrdenado");
        puts(" 12 - GravarListaArquivo");
        puts(" 13 - RecuperarListaArquivo");
        puts(" 14 - Criar novo arquivo limpo");
        puts("  0 - Voltar");
        puts("==============================================================");
        opcao = lerInteiro("Opcao: ");
        switch (opcao) {
        case 1:
            ListaInicializar();
            puts("Lista encadeada inicializada.");
            break;
        case 2:
            puts(ListaChecaLista() ? "ChecaLista: a lista possui elementos."
                                   : "ChecaLista: a lista esta vazia.");
            break;
        case 3: {
            int valor = lerInteiro("Valor para o fim: ");
            ListaAdicionaFim(valor);
            printf("AdicionaFim: %d inserido no fim.\n", valor);
            break;
        }
        case 4: {
            int valor = lerInteiro("Valor para o inicio: ");
            ListaAdicionaInicio(valor);
            printf("AdicionaInicio: %d inserido no inicio.\n", valor);
            break;
        }
        case 5:
            ListaMostraLista();
            break;
        case 6: {
            int valor = ListaRemoveInicio();
            if (valor != LISTA_SEM_VALOR)
                printf("RemoveInicio: %d removido do inicio.\n", valor);
            break;
        }
        case 7: {
            int valor = ListaRemoveFim();
            if (valor != LISTA_SEM_VALOR)
                printf("RemoveFim: %d removido do fim.\n", valor);
            break;
        }
        case 8:
            ListaLimpaLista();
            puts("LimpaLista: todos os elementos foram liberados.");
            break;
        case 9:
            printf("TamanhoDaLista: %d elemento(s).\n",
                   ListaTamanhoDaLista());
            break;
        case 10: {
            int posicao = lerInteiro("Posicao (base 0): ");
            int valor = ListaRemovePosicao(posicao);
            if (valor != LISTA_SEM_VALOR)
                printf("RemovePosicao: %d removido da posicao %d.\n", valor,
                       posicao);
            break;
        }
        case 11: {
            int valor = lerInteiro("Valor para ordenar: ");
            ListaAdicionarOrdenado(valor);
            printf("AdicionarOrdenado: %d inserido em ordem crescente.\n",
                   valor);
            break;
        }
        case 12:
            ListaGravarListaArquivo();
            break;
        case 13:
            ListaRecuperarListaArquivo();
            break;
        case 14:
            novoArquivoLimpo(LISTA_ARQUIVO, ListaLimpaLista,
                             ListaGravarListaArquivo);
            break;
        case 0:
            break;
        default:
            puts("Opcao invalida.");
            break;
        }
        if (opcao != 0)
            pausar();
    } while (opcao != 0);
}

static void menuListaDupla(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("LISTA DUPLAMENTE ENCADEADA", ListaDuplaTamanhoDaLista(),
                  LISTAD_ARQUIVO);
        ListaDuplaMostraLista();
        puts("--------------------------------------------------------------");
        puts("  1 - Inicializar");
        puts("  2 - ChecaLista");
        puts("  3 - AdicionaFim");
        puts("  4 - AdicionaInicio");
        puts("  5 - MostraLista");
        puts("  6 - RemoveInicio");
        puts("  7 - RemoveFim");
        puts("  8 - LimpaLista");
        puts("  9 - TamanhoDaLista");
        puts(" 10 - RemovePosicao");
        puts(" 11 - AdicionarOrdenado");
        puts(" 12 - GravarListaArquivo");
        puts(" 13 - RecuperarListaArquivo");
        puts(" 14 - Criar novo arquivo limpo");
        puts("  0 - Voltar");
        puts("==============================================================");
        opcao = lerInteiro("Opcao: ");
        switch (opcao) {
        case 1:
            ListaDuplaInicializar();
            puts("Lista duplamente encadeada inicializada.");
            break;
        case 2:
            puts(ListaDuplaChecaLista()
                     ? "ChecaLista: a lista possui elementos."
                     : "ChecaLista: a lista esta vazia.");
            break;
        case 3: {
            int valor = lerInteiro("Valor para o fim: ");
            ListaDuplaAdicionaFim(valor);
            printf("AdicionaFim: %d inserido no fim.\n", valor);
            break;
        }
        case 4: {
            int valor = lerInteiro("Valor para o inicio: ");
            ListaDuplaAdicionaInicio(valor);
            printf("AdicionaInicio: %d inserido no inicio.\n", valor);
            break;
        }
        case 5:
            ListaDuplaMostraLista();
            break;
        case 6: {
            int valor = ListaDuplaRemoveInicio();
            if (valor != LISTAD_SEM_VALOR)
                printf("RemoveInicio: %d removido do inicio.\n", valor);
            break;
        }
        case 7: {
            int valor = ListaDuplaRemoveFim();
            if (valor != LISTAD_SEM_VALOR)
                printf("RemoveFim: %d removido do fim.\n", valor);
            break;
        }
        case 8:
            ListaDuplaLimpaLista();
            puts("LimpaLista: todos os elementos foram liberados.");
            break;
        case 9:
            printf("TamanhoDaLista: %d elemento(s).\n",
                   ListaDuplaTamanhoDaLista());
            break;
        case 10: {
            int posicao = lerInteiro("Posicao (base 0): ");
            int valor = ListaDuplaRemovePosicao(posicao);
            if (valor != LISTAD_SEM_VALOR)
                printf("RemovePosicao: %d removido da posicao %d.\n", valor,
                       posicao);
            break;
        }
        case 11: {
            int valor = lerInteiro("Valor para ordenar: ");
            ListaDuplaAdicionarOrdenado(valor);
            printf("AdicionarOrdenado: %d inserido em ordem crescente.\n",
                   valor);
            break;
        }
        case 12:
            ListaDuplaGravarListaArquivo();
            break;
        case 13:
            ListaDuplaRecuperarListaArquivo();
            break;
        case 14:
            novoArquivoLimpo(LISTAD_ARQUIVO, ListaDuplaLimpaLista,
                             ListaDuplaGravarListaArquivo);
            break;
        case 0:
            break;
        default:
            puts("Opcao invalida.");
            break;
        }
        if (opcao != 0)
            pausar();
    } while (opcao != 0);
}

static void menuFila(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("FILA (FIFO)", FilaTamanhoDaLista(), FILA_ARQUIVO);
        FilaMostraLista();
        puts("--------------------------------------------------------------");
        puts("  1 - Inicializar");
        puts("  2 - ChecaLista");
        puts("  3 - Adiciona (enfileirar no fim)");
        puts("  4 - Remove (desenfileirar da frente)");
        puts("  5 - MostraLista");
        puts("  6 - LimpaLista");
        puts("  7 - TamanhoDaLista");
        puts("  8 - GravarListaArquivo");
        puts("  9 - RecuperarListaArquivo");
        puts(" 10 - Criar novo arquivo limpo");
        puts("  0 - Voltar");
        puts("==============================================================");
        opcao = lerInteiro("Opcao: ");
        switch (opcao) {
        case 1:
            FilaInicializar();
            puts("Fila inicializada.");
            break;
        case 2:
            puts(FilaChecaLista() ? "ChecaLista: a fila possui elementos."
                                  : "ChecaLista: a fila esta vazia.");
            break;
        case 3: {
            int valor = lerInteiro("Valor para enfileirar: ");
            FilaAdiciona(valor);
            printf("Adiciona: %d entrou no fim da fila.\n", valor);
            break;
        }
        case 4: {
            int valor = FilaRemove();
            if (valor != FILA_SEM_VALOR)
                printf("Remove: %d saiu da frente da fila.\n", valor);
            break;
        }
        case 5:
            FilaMostraLista();
            break;
        case 6:
            FilaLimpaLista();
            puts("LimpaLista: todos os elementos foram liberados.");
            break;
        case 7:
            printf("TamanhoDaLista: %d elemento(s).\n", FilaTamanhoDaLista());
            break;
        case 8:
            FilaGravarListaArquivo();
            break;
        case 9:
            FilaRecuperarListaArquivo();
            break;
        case 10:
            novoArquivoLimpo(FILA_ARQUIVO, FilaLimpaLista,
                             FilaGravarListaArquivo);
            break;
        case 0:
            break;
        default:
            puts("Opcao invalida.");
            break;
        }
        if (opcao != 0)
            pausar();
    } while (opcao != 0);
}

static void menuPilha(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("PILHA (LIFO)", PilhaTamanhoDaLista(), PILHA_ARQUIVO);
        PilhaMostraLista();
        puts("--------------------------------------------------------------");
        puts("  1 - Inicializar");
        puts("  2 - ChecaLista");
        puts("  3 - Adiciona (empilhar no topo)");
        puts("  4 - Remove (desempilhar do topo)");
        puts("  5 - MostraLista");
        puts("  6 - LimpaLista");
        puts("  7 - TamanhoDaLista");
        puts("  8 - GravarListaArquivo");
        puts("  9 - RecuperarListaArquivo");
        puts(" 10 - Criar novo arquivo limpo");
        puts("  0 - Voltar");
        puts("==============================================================");
        opcao = lerInteiro("Opcao: ");
        switch (opcao) {
        case 1:
            PilhaInicializar();
            puts("Pilha inicializada.");
            break;
        case 2:
            puts(PilhaChecaLista() ? "ChecaLista: a pilha possui elementos."
                                  : "ChecaLista: a pilha esta vazia.");
            break;
        case 3: {
            int valor = lerInteiro("Valor para empilhar: ");
            PilhaAdiciona(valor);
            printf("Adiciona: %d foi para o topo da pilha.\n", valor);
            break;
        }
        case 4: {
            int valor = PilhaRemove();
            if (valor != PILHA_SEM_VALOR)
                printf("Remove: %d saiu do topo da pilha.\n", valor);
            break;
        }
        case 5:
            PilhaMostraLista();
            break;
        case 6:
            PilhaLimpaLista();
            puts("LimpaLista: todos os elementos foram liberados.");
            break;
        case 7:
            printf("TamanhoDaLista: %d elemento(s).\n",
                   PilhaTamanhoDaLista());
            break;
        case 8:
            PilhaGravarListaArquivo();
            break;
        case 9:
            PilhaRecuperarListaArquivo();
            break;
        case 10:
            novoArquivoLimpo(PILHA_ARQUIVO, PilhaLimpaLista,
                             PilhaGravarListaArquivo);
            break;
        case 0:
            break;
        default:
            puts("Opcao invalida.");
            break;
        }
        if (opcao != 0)
            pausar();
    } while (opcao != 0);
}

int main(void)
{
    prepararDiretorio();
    int opcao;
    do {
        limparTela();
        puts("==============================================================");
        puts("  SISTEMA DE ESTRUTURAS DE DADOS EM C");
        puts("  Listas encadeadas, listas duplas, filas e pilhas");
        puts("==============================================================");
        puts("  1 - Lista encadeada");
        puts("  2 - Lista duplamente encadeada");
        puts("  3 - Fila");
        puts("  4 - Pilha");
        puts("  0 - Sair");
        puts("==============================================================");
        opcao = lerInteiro("Escolha a estrutura ou 0 para sair: ");
        switch (opcao) {
        case 1:
            ListaInicializar();
            menuLista();
            break;
        case 2:
            ListaDuplaInicializar();
            menuListaDupla();
            break;
        case 3:
            FilaInicializar();
            menuFila();
            break;
        case 4:
            PilhaInicializar();
            menuPilha();
            break;
        case 0:
            break;
        default:
            puts("Opcao invalida.");
            pausar();
            break;
        }
    } while (opcao != 0);
    puts("");
    puts("Ate logo!");
    return 0;
}
