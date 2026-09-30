# Sistema de Estruturas de Dados em C

Material de estudo: como funcionam, por dentro, as quatro estruturas implementadas
aqui — **lista encadeada simples**, **lista duplamente encadeada**, **fila (FIFO)**
e **pilha (LIFO)** — usando apenas a linguagem C, sem biblioteca de estruturas.

O projeto foi feito para ser lido da esquerda para a direita: `main.c` é a
interface com o usuário, e cada estrutura mora em um par de arquivos próprios
(`.c` com a implementação, `.h` com o contrato).

---

## Sumário

1. [Como compilar e executar](#1-como-compilar-e-executar)
2. [Mapa do projeto](#2-mapa-do-projeto)
3. [Conceitos prévios indispensáveis](#3-conceitos-prévios-indispensáveis)
4. [O formato do arquivo binário](#4-o-formato-do-arquivo-binário)
5. [Lista encadeada simples](#5-lista-encadeada-simples-listac)
6. [Lista duplamente encadeada](#6-lista-duplamente-encadeada-listaduplac)
7. [Fila (FIFO)](#7-fila-fifo-filac)
8. [Pilha (LIFO)](#8-pilha-lifo-pilhac)
9. [O programa principal](#9-o-programa-principal-mainc)
10. [Tabela de complexidades](#10-tabela-de-complexidades)
11. [Armadilhas clássicas de prova](#11-armadilhas-clássicas-de-prova)
12. [Autoavaliação (com gabarito no final da seção)](#12-autoavaliação)

---

## 1. Como compilar e executar

```bash
gcc -Wall -Wextra -O2 -o app.exe main.c lista.c listaDupla.c fila.c pilha.c
.\app.exe
```

No Linux/macOS o alvo é `app` em vez de `app.exe`. Se você tiver `make`:

```bash
make run
```

Observações importantes:

- O `.exe` é **portátil**: ele não depende de nada além da `libc`. Os arquivos de
  dados usam caminho **relativo** (`dados/...`), então a pasta `dados/` é criada
  automaticamente na primeira execução, no diretório de onde você chamou o
  programa. Se você copiar o `.exe` para outra pasta e rodar lá, ele cria uma
  `dados/` nova, do zero.
- `-Wall -Wextra` liga avisos de compilação. O projeto compira **sem nenhum aviso**;
  se aparecer algum no seu, provavelmente está faltando um `free` ou um retorno
  de erro que você não checou.

---

## 2. Mapa do projeto

| Arquivo | Tipo do nó | Variável global | Papel |
|---|---|---|---|
| `main.c` | — | — | Telas de menu, entrada do usuário, mensagens |
| `lista.c` / `lista.h` | `No { valor; prox; }` | `No *cabecaLista` | Lista encadeada simples |
| `listaDupla.c` / `listaDupla.h` | `NoDuplo { valor; prox; ant; }` | `NoDuplo *cabecaListaDupla` | Lista duplamente encadeada |
| `fila.c` / `fila.h` | `NoFila { valor; prox; }` | `NoFila *traseiraFila` | Fila (FIFO) |
| `pilha.c` / `pilha.h` | `NoPilha { valor; prox; }` | `NoPilha *topoPilha` | Pilha (LIFO) |
| `Makefile` | — | — | Atalho de compilação |

### Por que os nomes das funções têm prefixo?

Você pediu as funções com o mesmo nome nas quatro estruturas
(`AdicionaFim`, `RemoveInicio`, ...). Isso é impossível em C: **C não tem
namespaces**. Se `lista.c` e `pilha.c` tivessem ambos uma função `MostraLista`,
o linker encontraria dois símbolos com o mesmo nome e o programa não linkaria
(`multiple definition of 'MostraLista'`).

A solução adotada é prefixar cada função com o nome da estrutura:

```
ListaAdicionaFim        ListaDuplaAdicionaFim
ListaRemoveInicio       ListaDuplaRemoveInicio
FilaAdiciona            PilhaAdiciona
```

No menu, o usuário continua vendo exatamente o nome que você pediu
(`AdicionaFim`, `RemovePosicao`, ...). O prefixo existe só no **símbolo do
compilador**, para manter os arquivos independentes. A alternativa seria
declarar todas as funções como `static`, mas aí elas morreriam dentro do próprio
arquivo e o `main.c` não poderia chamá-las.

### Por que a cabeça da lista é uma variável global?

Cada arquivo guarda o estado da sua estrutura num único ponteiro no topo do
arquivo:

```c
No *cabecaLista = NULL;
```

Consequência prática: as funções **não recebem** a cabeça por parâmetro. Todas
 mexem na mesma variável. Vantagens para o estudo: você vê o estado da estrutura
numa linha só, e não precisaolar o "estado da lista" entre chamadas. O custo é
justo: o estado é global, então duas listas do mesmo tipo não podem coexistir
(comportamento idêntico ao de uma variável global comum).

A declaração `extern` no `.h` é o que exporta esse ponteiro para os outros
arquivos:

```c
/* lista.h */
extern No *cabecaLista;   /* "a variável existe, definida em outro lugar" */
```

```c
/* lista.c */
No *cabecaLista = NULL;   /* a definição real, com memória reservada */
```

`extern` não reserva memória: ele apenas promete ao compilador que a variável
está definida em outro arquivo. Sem o `extern` no `.h`, o `main.c` não conseguiria
nem ler o tamanho da lista.

---

## 3. Conceitos prévios indispensáveis

### 3.1 Por que `malloc` e não `No novo;`

Um nó precisa existir em memória **heap**, porque seu endereço tem de ser
guardado dentro de outro nó e sobreviver ao fim da função que o criou:

```c
static No *criarNo(int valor)
{
    No *novo = malloc(sizeof *novo);   /* sizeof *novo = sizeof(No), sem depender do nome do tipo */
    if (novo == NULL) {                /* malloc pode falhar se não houver memória */
        puts("Erro: memoria insuficiente para alocar o no.");
        return NULL;
    }
    novo->valor = valor;
    novo->prox = NULL;                 /* nó nasce apontando para o fim da lista */
    return novo;
}
```

Três pontos que caem em prova:

- `sizeof *novo` em vez de `sizeof(No)`: se o tipo mudar um dia, o código continua
  correto, porque mede o tamanho do objeto real.
- `malloc` **não inicializa** a memória. Por isso definimos `prox = NULL` na mão.
  Se esquecermos, a lista ganha um ponteiro lixo e o programa quebra ou entra em
  laço infinito.
- O `if (novo == NULL)` é obrigatório. Sem ele, um estouro de memória viraria
  *segmentation fault* no `novo->valor = valor`.

Esse mesmo `criarNo` é repetido (com nomes diferentes: `No`, `NoDuplo`, `NoFila`,
`NoPilha`) nos quatro arquivos. É duplicação proposital: cada arquivo é
independente, e o tipo do nó é diferente em cada um.

### 3.2 O padrão "guarde o próximo, depois libere"

Toda função que percorre a lista para liberar nós repete este par de linhas:

```c
No *proximo = atual->prox;   /* 1. salva o endereço do vizinho ANTES de liberar */
free(atual);                 /* 2. agora pode liberar o nó atual */
atual = proximo;             /* 3. e pula para o próximo, que continua válido */
```

Se você inverter a ordem (`free(atual); atual = atual->prox;`), está usando
memória **já liberada**: o comportamento é indefinido. Na prática, o conteúdo
costuma continuar ali por acaso (o `free` raramente zera a memória), então o
programa "funciona" no seu computador e quebra no da prova. Esse é o erro
clássico número um.

O mesmo cuidado vale com `free` em geral: depois de `free(x)`, o valor de `x` é
lixo. Guarde o conteúdo que você precisa (`int valor = removido->valor;`)
**antes** de liberar.

### 3.3 `static` em duas quebras de sentido diferentes

No projeto, `static` aparece com dois papéis que você precisa diferenciar:

- `static int inicializada = 0;` — variável **privada ao arquivo**, uma "memória"
  do módulo. Existe uma cópia só, e só este arquivo enxerga. Serve de flag para
  a inicialização ser feita uma única vez.
- `static No *criarNo(int valor)` — função **privada ao arquivo**: funciona
  exatamente igual, mas o linker não exporta o nome. É o "funcionário interno".

### 3.4 O flag `inicializada`

```c
void ListaInicializar(void)
{
    if (inicializada)
        return;          /* já foi inicializada: não apaga nada */
    cabecaLista = NULL;
    inicializada = 1;
}
```

Por que isso? Porque o programa chama `ListaInicializar()` **todas as vezes que
você escolhe a estrutura no menu**. Se `Inicializar` apenas fizesse
`cabecaLista = NULL`, toda ida e volta ao menu apagaria o que você tinha feito.
Com o flag, a **primeira** chamada prepara a estrutura (que já nasce vazia, por
causa do `= NULL` na declaração) e as seguintes são inócuas. O resultado é o
comportamento que você pediu: sair do menu com `0` e voltar **preserva os dados**.

---

## 4. O formato do arquivo binário

`GravarListaArquivo` e `RecuperarListaArquivo` existem nas quatro estruturas e
funcionam igual, mudando só as assinaturas. O formato é binário puro
(`fwrite`/`fread`), em três partes:

```
[ assinatura : 8 bytes ][ quantidade : sizeof(int) ][ valor 1 ][ valor 2 ] ... [ valor n ]
```

Exemplo real: a lista `5 -> 20 -> 30` gravada em `dados/lista_encadeada.dat`
produziu exatamente 24 bytes:

```
4C 49 53 54 41 30 30 31  03 00 00 00  05 00 00 00  14 00 00 00  1E 00 00 00
└──── "LISTA001" ────┘  └ count=3 ┘  └ valor 5 ┘  └ valor 20┘  └ valor 30┘
```

Lendo da esquerda para a direita:

- `4C 49 53 54 41 30 30 31` são os bytes de `"LISTA001"` (cada caractere ASCII
  ocupa 1 byte). Essa é a **assinatura**: ela impede que você recupere, por
  engano, um arquivo de outra estrutura (as assinaturas são `LISTA001`, `LISTD001`,
  `FILA0001` e `PILH0001`).
- `03 00 00 00` é o `int` 3 gravado no formato nativo little-endian do Windows.
  É a **quantidade** de elementos: ela permite reservar o vetor certo e, principalmente,
  **detectar arquivo truncado** na leitura.
- `05 00 00 00`, `14 00 00 00`, `1E 00 00 00` são os valores 5, 20 e 30, um `int`
  cada.

Repare que **não gravamos os ponteiros**. Isso é essencial: ponteiros só fazem
sentido dentro do processo que os criou. Gravar `&no` num arquivo e tentar
recuperar em outro programa daria acesso a memória inválida. O que se grava é
só o **conteúdo**; a lista é **reconstruída** com `malloc` na leitura.

### 4.1 `fwrite` e `fread` têm 4 argumentos

```c
fwrite(ORIGEM, TAMANHO_DE_CADA_ITEM, QUANTOS_ITENS, ARQUIVO);
fread (DESTINO, TAMANHO_DE_CADA_ITEM, QUANTOS_ITENS, ARQUIVO);
```

Há duas granularidades em uso no projeto, e misturá-las é erro clássico de prova:

- **Escrevendo a assinatura**, a unidade é o **byte** (`1`), então o terceiro
  argumento vira "quantos bytes":

```c
fwrite(LISTA_ASSINATURA, 1, sizeof(LISTA_ASSINATURA) - 1, arquivo);
```

- **Escrevendo os valores**, a unidade é o **próprio `int`** (`sizeof(int)`) e o
  terceiro argumento é sempre `1` (um único inteiro):

```c
fwrite(&atual->valor, sizeof(int), 1, arquivo);
```

Usar `sizeof(int)` em vez do número `4` deixa o código independente da
plataforma: o `int` não tem 4 bytes em todos os sistemas.

`sizeof(LISTA_ASSINATURA)` é 9 (8 letras + o `'\0'`), por isso o `- 1`: nós
queremos os 8 bytes da assinatura, **sem** o terminador nulo.

### 4.2 Checar o retorno de `fwrite`/`fread` é obrigatório

`fwrite` e `fread` devolvem **quantos itens realmente foram transferidos**. Se o
disco estiver cheio ou o arquivo acabar no meio, elas devolvem menos do que o
pedido. O código compara:

```c
if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 || quantidade < 0) {
    puts("Arquivo corrompido: quantidade de elementos invalida.");
    fclose(arquivo);
    return 0;
}
```

Se você não checar, o programa segue adiante com `quantidade` contendo lixo e
tenta alocar um vetor gigante (ou faz `malloc` de um valor negativo, que
"funciona" e corrompe tudo).

### 4.3 A leitura em duas fases

`RecuperarListaArquivo` **não** limpa a lista antes de saber se o arquivo é
válido. O fluxo é:

1. Abrir e validar assinatura e contador.
2. Ler todos os valores para um vetor temporário (`malloc` de `quantidade` ints).
3. Só depois de o arquivo inteiro ter sido lido com sucesso: `LimpaLista()` e
   reinsere os valores com `AdicionaFim`.

Assim, se o arquivo estiver corrompido ou truncado, **a lista que estava na
memória continua intacta**. O vetor temporário é devolvido com `free(valores)`.

Uma observação importante para a **pilha**: o arquivo guarda os elementos do topo
para a base. Para reconstruir, é preciso empilhar na **ordem inversa**:

```c
for (int i = quantidade - 1; i >= 0; i--)
    PilhaAdiciona(valores[i]);
```

Sem inverter, a pilha viraria de cabeça para baixo ao ser recuperada. Na fila e
nas listas o problema não existe, porque a inserção é sempre no fim.

---

## 5. Lista encadeada simples (`lista.c`)

### O modelo mental

A lista é uma cadeia de nós, e a variável global `cabecaLista` é a entrada.
O fim da lista é sinalizado por `prox == NULL`.

```
   cabecaLista
        |
        v
     +------+------+      +------+------+      +------+------+
     | 10   |  o----+---> | 20   |  o-- +---> | 30   | NULL |
     +------+------+      +------+------+      +------+------+
       prox                                        prox
```

Para três nós e um `int`, o tipo ocupa 8 bytes (4 do `int` + 4 do ponteiro), logo
a lista acima consome 24 bytes de heap — e o `free` precisa devolvê-los.

### `ListaInicializar(void)`

Prepara a estrutura uma única vez (ver [3.4](#34-o-flag-inicializada)). Não
devolve nada: `void` significa "não há resultado", e o estado alterado fica na
variável global.

### `ListaChecaLista(void) -> int`

```c
return (cabecaLista != NULL);
```

O teste de "lista vazia" em lista encadeada é **sempre** `cabeca == NULL`. Não
existe contador, não existe nó sentinela. (A fila é diferente: veja
[seção 7](#7-fila-fifo-filac).)

### `ListaAdicionaFim(int valor)`

```c
if (cabecaLista == NULL) {   /* lista vazia: o novo nó É a cabeça */
    cabecaLista = novo;
    return;
}
No *atual = cabecaLista;
while (atual->prox != NULL)   /* caminha até o último nó */
    atual = atual->prox;
atual->prox = novo;           /* costura o novo nó no fim */
```

O `if` do começo não é otimização: sem ele, `atual` seria `NULL` e
`atual->prox` causaria *segmentation fault*. Esse laço é **O(n)** — o preço de
uma lista sem ponteiro para a cauda. (Guardar um `ultimo` global deixaria O(1),
mas aí remover do fim deixaria de ser O(1).)

### `ListaAdicionaInicio(int valor)`

```c
novo->prox = cabecaLista;   /* o novo nó aponta para a velha cabeça */
cabecaLista = novo;         /* e vira a nova cabeça */
```

**Duas linhas, O(1).** Essa é a grande vantagem da lista encadeada. A ordem
dessas duas linhas é obrigatória: inverter faria o novo nó apontar para si
mesmo. Costuma ser o primeiro bug que o professor cobra.

### `ListaMostraLista(void)`

```c
for (No *atual = cabecaLista; atual != NULL; atual = atual->prox)
    printf(" %d%s", atual->valor, atual->prox != NULL ? " ->" : "");
```

O `for` acima é o **caminhamento padrão** de lista: começa na cabeça, avança por
`prox`, termina no `NULL`. Compare com `atual->prox != NULL ? " ->" : ""`: isso
evita imprimir uma seta no final. É um operador ternário dentro do `printf`.

### `ListaRemoveInicio(void) -> int`

```c
No *removido = cabecaLista;      /* guarda o nó que vai morrer */
cabecaLista = removido->prox;    /* a cabeça pula para o vizinho */
int valor = removido->valor;     /* SALVA o valor antes do free */
free(removido);
return valor;
```

Três ordens que não podem mudar: buscar o valor **antes** de liberar, liberar
**depois** de religar a cabeça, e nunca tocar em `removido` depois do `free`.

### `ListaRemoveFim(void) -> int`

```c
if (cabecaLista->prox == NULL) { /* a lista tem 1 nó só */
    int valor = cabecaLista->valor;
    free(cabecaLista);
    cabecaLista = NULL;          /* senão a cabeça fica dangling */
    return valor;
}
No *atual = cabecaLista;
while (atual->prox->prox != NULL)  /* vai até o PENÚLTIMO */
    atual = atual->prox;
int valor = atual->prox->valor;
free(atual->prox);
atual->prox = NULL;                 /* o penúltimo vira o novo último */
return valor;
```

Repare no `atual->prox->prox != NULL`: o laço para quando o nó atual tem um
vizinho **depois** dele, ou seja, quando `atual` é o penúltimo. Assim
`atual->prox` é sempre o nó a remover. Remover do fim custa **O(n)** porque não
existe atalho para a cauda. O caso especial de 1 nó é obrigatório: sem ele, o
laço iniciaria com `cabecaLista->prox` já `NULL` e o `->prox` do `NULL` quebraria
o programa.

### `ListaLimpaLista(void)`

```c
No *atual = cabecaLista;
while (atual != NULL) {
    No *proximo = atual->prox;
    free(atual);
    atual = proximo;
}
cabecaLista = NULL;   /* fundamental: senão a cabeça vira memória liberada */
```

Terminar com `cabecaLista = NULL` é obrigatório em **qualquer** função que
libere o último nó. Esquecer disso é *use-after-free* na cabeça.

### `ListaTamanhoDaLista(void) -> int`

Só conta, andando do começo ao fim. É **O(n)** e não existe contador de tamanho
na estrutura — se houvesse, `TamanhoDaLista` seria O(1), mas toda inserção e
remoção ganharia trabalho extra.

### `ListaRemovePosicao(int posicao) -> int`

A função mais instructive do arquivo. Ela localiza o nó e seu antecessor
caminhando **uma vez só**, carregando dois ponteiros:

```c
No *anterior = NULL;
No *atual = cabecaLista;
int indice = 0;
while (atual != NULL && indice < posicao) {
    anterior = atual;        /* o anterior anda junto, sempre 1 passo atrás */
    atual = atual->prox;
    indice++;
}
if (atual == NULL) {  /* o laço andou além do fim */
    printf("Posicao %d fora do intervalo [0, %d].\n", posicao,
           ListaTamanhoDaLista() - 1);
    return LISTA_SEM_VALOR;
}
if (anterior != NULL)
    anterior->prox = atual->prox;   /* costura: pula o nó removido */
else
    cabecaLista = atual->prox;       /* era a posição 0: a cabeça muda */
int valor = atual->valor;
free(atual);
return valor;
```

Quatro detalhes para a prova:

1. **Base 0.** Posição 0 é a cabeça. A posição válida vai de `0` até
   `TamanhoDaLista() - 1`, e é isso que a mensagem de erro mostra.
2. **`anterior` começa `NULL`.** Quando `posicao == 0`, o laço não roda nenhuma
   vez, então `anterior` continua `NULL`. O `if (anterior != NULL)` é o que
   distingue "remover a cabeça" de "remover o meio". Sem esse `if`, a cabeça
   nunca seria removida corretamente.
3. **Validação em duas camadas.** Primeiro `posicao < 0` é rejeitado com
   mensagem própria; depois, se o laço terminou em `atual == NULL`, a posição
   era grande demais. Uma única checagem no fim não distinguiria os dois erros.
4. **`LISTA_SEM_VALOR`.** A função devolve `int`, e um `int` de retorno não tem
   como dizer "não deu certo". A solução do projeto é um **valor sentinela**:
   `#define LISTA_SEM_VALOR INT_MAX` (o maior `int` representável). O menu
   compara o retorno com o sentinela para não anunciar uma remoção que não
   aconteceu:

   ```c
   int valor = ListaRemovePosicao(posicao);
   if (valor != LISTA_SEM_VALOR)
       printf("RemovePosicao: %d removido da posicao %d.\n", valor, posicao);
   ```

    A alternativa seria passar um `int *saida` por parâmetro e usar o retorno
   só para indicar sucesso/falha — mais explícito, mas muda a assinatura.

### `ListaAdicionarOrdenado(int valor)`

Inserção mantendo ordem **crescente**, **aceitando repetidos**:

```c
if (cabecaLista == NULL || valor < cabecaLista->valor) {
    novo->prox = cabecaLista;    /* é o menor: vira a cabeça */
    cabecaLista = novo;
    return;
}
No *atual = cabecaLista;
while (atual->prox != NULL && atual->prox->valor <= valor)
    atual = atual->prox;        /* pula todos os <= valor */
novo->prox = atual->prox;      /* insere entre atual e o próximo */
atual->prox = novo;
```

O detalhe fino é o **`<=`** no laço, e não `<`. Com `<=`, a varredura ultrapassa
todos os nós iguais a `valor`, e o novo nó é inserido **depois** deles. Isso
mantém a ordem entre elementos repetidos (ordem estável). O efeito colateral
esperado: se você digita `5`, `3`, `7`, `5`, `-2`, `5` nessa ordem, a lista
resultante é `-2 -> 3 -> 5 -> 5 -> 5 -> 7`.

É **O(n)** por natureza: lista encadeada não tem busca binária.

### `ListaGravarListaArquivo(void) -> int` e `ListaRecuperarListaArquivo(void) -> int`

Já detalhados na [seção 4](#4-o-formato-do-arquivo-binário). A assinatura
retorna `1` em sucesso e `0` em falha, para que o menu possa reagir.

---

## 6. Lista duplamente encadeada (`listaDupla.c`)

### O que muda

O nó ganha um ponteiro a mais, `ant`, e **todo** ponteiro continua sendo uma
lista com cabeça em `cabecaListaDupla`.

```c
typedef struct NoDuplo {
    int valor;
    struct NoDuplo *prox;   /* próximo (direita) */
    struct NoDuplo *ant;    /* anterior (esquerda) */
} NoDuplo;
```

```
        cabecaListaDupla
              |
              v
      +------+------+------+      +------+------+------+      +------+------+------+
      | 10   |  o--|----o-+---> | 20   |  o--|----o +---> | 30   | NULL |  o   |
      +------+------+------+      +------+------+------+      +------+------+------+
        ant  \     |  / prox        ant  \     |  /prox        ant  \    |  /prox
             <------+                        <------                    <-------
```

### Os quatro invariantes que precisam valer SEMPRE

Este é o coração da matéria. Em qualquer estado válido da lista:

| # | Invariante |
|---|---|
| 1 | `cabecaListaDupla->ant == NULL` (a cabeça não tem anterior) |
| 2 | O último nó tem `prox == NULL` |
| 3 | Para todo nó `n` com `n->prox != NULL`: `n->prox->ant == n` |
| 4 | Para todo nó `n` com `n->ant != NULL`: `n->ant->prox == n` |

O item 4 é o que realmente distingue a lista dupla: navegar para trás é tão
barato quanto para frente, sem precisar saber onde é o fim.

### As funções que mudam de verdade

O `AdicionaFim` ganha uma linha (`novo->ant = atual;`) e o `AdicionaInicio`
ganha um `if`:

```c
/* AdicionaFim */
atual->prox = novo;
novo->ant = atual;                 /* <- só isso */

/* AdicionaInicio */
novo->prox = cabecaListaDupla;
if (cabecaListaDupla != NULL)       /* <- sem isso, a cabeça ganha ant lixo */
    cabecaListaDupla->ant = novo;
cabecaListaDupla = novo;
```

As remoções precisam **religar o vizinho que sobra**:

```c
/* RemovePosicao */
if (anterior != NULL)
    anterior->prox = atual->prox;
else
    cabecaListaDupla = atual->prox;
if (atual->prox != NULL)            /* <- conserta o ant do sucessor */
    atual->prox->ant = anterior;
int valor = atual->valor;
free(atual);
```

Repare que `atual->prox->ant = anterior` funciona **inclusive quando
`anterior == NULL`**: nesse caso o sucessor vira a nova cabeça e o `ant` dele tem
que ser `NULL`. É a mesma expressão tratando os dois casos — elegantíssimo e
frequentemente pedido em prova.

`RemoveInicio` precisa do mesmo cuidado, e `RemoveFim` não precisa de nada
extra, porque remover a cauda só afeta o `prox` do penúltimo, e o `ant` do nó
removido morre junto com ele.

`MostraLista` só muda o separador, de `->` para `<->`.

### Como verificar os invariantes na prática

Este é o exercício que vale a pena refazer antes da prova: um laço que caminha
para trás a partir do **último** nó e confere que `atual->ant->prox == atual` em
todos eles. A pegadinha é que, como não existe ponteiro para a cauda, primeiro
você caminha para frente achando o último, e só então começa a voltar. Caminhar
para trás começando pela cabeça não funciona — a cabeça não tem `ant`, então você
visita 1 nó e para.

---

## 7. Fila (FIFO) (`fila.c`)

### FIFO em uma frase

*Primeiro que entrou, primeiro que sai.* `Adiciona` entra pelo fim,
`Remove` sai pela frente.

### A dificuldade: são dois extremos, mas você quer uma variável só

Uma fila precisa andar em **duas direções**: a frente (para tirar) e a
traseira (para colocar). Uma lista simples com uma variável `cabeca` resolve a
frente em O(1), mas para alcançar a traseira teria de percorrer tudo — O(n) por
enfileiramento. A solução clássica (Kernighan & Ritchie) é usar uma **lista
circular** e guardar o ponteiro no **fim**:

```
   vazia:  traseiraFila == NULL

   1 elemento:      traseiraFila ──┐ (o nó aponta para si mesmo)
                        ↑  +───────┘
                        └── o nó é a frente E a traseira

   3 elementos:   traseiraFila
                        ↑    |
                   ┌────┼────┼────┐
                   ▼    |    |    |
                  [30]--+   [20]  [10]
                   ↑             │
                   └─────────────┘
              (frente = traseiraFila->prox = 10)
```

Regras que decorram:

- **Vazia** ⟺ `traseiraFila == NULL`.
- **Frente** = `traseiraFila->prox` (a função `frente()` só embrulha isso).
- **1 elemento** ⟺ o nó aponta para si mesmo (`traseiraFila->prox == traseiraFila`).
- A lista **não tem NULL no meio**: ela é circular, e o fim é a traseira.

Assim, `Adiciona` e `Remove` são **O(1)** usando uma única variável.

### `FilaAdiciona(int valor)` — o detalhe que custou um bug

```c
if (filaVazia()) {          /* fila vazia: o novo nó é frente e traseira */
    novo->prox = novo;
    traseiraFila = novo;
    return;
}
novo->prox = traseiraFila->prox;   /* 1. o novo aponta para a frente atual */
traseiraFila->prox = novo;         /* 2. a traseira antiga aponta para o novo */
traseiraFila = novo;               /* 3. o novo vira a nova traseira */
```

A ideia é: o novo nó entra **entre a traseira antiga e a frente**. Ao final, ele
é a nova traseira. Repare que a **traseira muda de endereço** — por isso ela é
uma variável que precisa ser reatribuída, e não um nó fixo.

> **Armadilha histórica deste projeto:** a primeira versão usava uma sentinela
> real apontando para a **frente** e inseria com `cabecaFila->prox->prox = novo`,
> achando que essa era a traseira. Ela só é a traseira quando a fila tem
> **1 elemento**; com 2 ou mais, a 3ª inserção sobrescrevia o link e perdia um
> nó silenciosamente. O sintoma era a fila exibindo `100 -> 300` depois de
> enfileirar 100, 200 e 300. O teste automatizado pegou isso; os testes de tela,
> não. Vale como lição: **a forma do código (2 linhas) estava errada mesmo
> passando em quase todos os casos.**

### `FilaRemove(void) -> int`

```c
NoFila *primeiro = frente();
int valor = primeiro->valor;      /* salva antes de liberar */
if (primeiro == traseiraFila) {   /* era o único elemento */
    free(primeiro);
    traseiraFila = NULL;          /* a fila volta ao estado "vazia" */
    return valor;
}
traseiraFila->prox = primeiro->prox;   /* a frente pula para o segundo */
free(primeiro);
return valor;
```

Note que **não existe "último elemento"**: quando a fila tem 1 só nó, ele é
simultaneamente a frente e a traseira, e o `if` devolve a fila ao estado vazio.
Sem esse `if`, o laço de exibição entraria em circulate infinito.

### `FilaMostraLista`, `FilaTamanhoDaLista`, `FilaGravarListaArquivo`: o laço circular

Como a lista não termina em `NULL`, o laço tradicional **não serve**. Não existe
`atual != NULL` para parar, e usar `atual != traseiraFila` também não dá certo
no caso de 1 elemento (a frente *é* a traseira, então o corpo nunca rodaria).
A solução é imprimir o nó e **depois** testar se ele era a traseira:

```c
for (NoFila *atual = frente();; atual = atual->prox) {
    printf(" %d", atual->valor);
    if (atual == traseiraFila)
        break;          /* imprimiu a traseira: acabou */
    printf(" ->");
}
```

Um `for` sem condição e sem incremento escrito, com o `break` dentro — é o
jeito padrão de percorrer estrutura circular. (A alternativa seria contar com
`FilaTamanhoDaLista()` e dar um `for` com contador.)

`FilaLimpaLista` libera andando da frente até a traseira e depois libera a
traseira, que é o único nó que sobrou.

### `FilaChecaLista` e `FilaInicializar`

`FilaChecaLista` é `!filaVazia()`, e `filaVazia()` é literalmente
`traseiraFila == NULL`. Repare que o teste de vazio da fila é diferente do das
listas, porque aqui o vazio é `NULL` e o "1 elemento" é um nó apontando para si
mesmo.

---

## 8. Pilha (LIFO) (`pilha.c`)

### LIFO em uma frase

*Último que entrou, primeiro que sai.* `Adiciona` empilha no topo, `Remove`
desempilha do topo. É a estrutura mais simples: **não tem dois extremos**, só um.

### `PilhaAdiciona(int valor)` — 2 linhas, O(1)

```c
novo->prox = topoPilha;   /* o novo empurra o topo antigo para baixo */
topoPilha = novo;
```

É literalmente o mesmo código de `ListaAdicionaInicio`. É por isso que a pilha é
a estrutura mais barata: só o topo importa, e ele está sempre à mão.

### `PilhaRemove(void) -> int`

```c
NoPilha *removido = topoPilha;
topoPilha = removido->prox;   /* o novo topo é o que estava embaixo */
int valor = removido->valor;
free(removido);
return valor;
```

Espelha `ListaRemoveInicio`. Nunca há caso especial de "último elemento": se havia
1 nó, `removido->prox` já era `NULL`, e o `topoPilha = NULL` sai de graça.

### Diferença de perspectiva na exibição

`PilhaMostraLista` imprime `topo -> base` (do mais novo para o mais antigo), e
`FilaMostraLista` imprime `frente -> traseira`. É a mesma lista encadeada
percorrida na direção que serve ao propósito de cada estrutura. Consequência
prática na recuperação do arquivo: a pilha é salva de cima para baixo, por isso
a leitura reinsere em ordem inversa (`for (i = quantidade - 1; i >= 0; i--)`).

---

## 9. O programa principal (`main.c`)

`main.c` não conhece nenhum detalhe de lista encadeada: ele só chama as funções
públicas e imprime o que o usuário digita. Isso é **encapsulamento** — o menu
não sabe nem se a lista é simples ou dupla, só existe o contrato declarado no
`.h`.

### Tela 1: seleção da estrutura

```
1 - Lista encadeada
2 - Lista duplamente encadeada
3 - Fila
4 - Pilha
0 - Sair
```

Um `do { ... } while (opcao != 0)` redesenha a tela a cada volta. Cada `case`
chama o `XInicializar()` da estrutura e depois o menu dela.

### Tela 2: menu da estrutura

Antes das opções, `cabecalho()` imprime os **metadados**: título, tamanho atual,
caminho do arquivo e se ele existe (`arquivoExiste` faz um `fopen` em modo `"rb"`
e fecha). Em seguida o menu lista as 14 (ou 10, na fila/pilha) operações mais o
`0 - Voltar`. É um `do...while` igual ao da tela 1.

Como a forma do menu é idêntica nas quatro estruturas e só as funções mudam,
o `switch (opcao)` é o mecanismo que amarra a tecla ao comportamento. Uma
observação de projeto: cada `case` que recebe valor do usuário declara uma
variável local entre chaves, porque em C uma declaração não pode virar o
primeiro comando de um `case`:

```c
case 3: {
    int valor = lerInteiro("Valor para o fim: ");
    ListaAdicionaFim(valor);
    printf("AdicionaFim: %d inserido no fim.\n", valor);
    break;
}
```

### As funções auxiliares de `main.c`

**`lerInteiro(const char *mensagem) -> int`** — o coração da robustez do menu.
Usa `fgets` (não `scanf`, que deixa lixo no buffer quando o usuário digita
letras), e `strtol` para converter, com validação:

```c
char *fim;
long convertido = strtol(linha, &fim, 10);
while (*fim == ' ' || *fim == '\t' || *fim == '\n' || *fim == '\r')
    fim++;
if (fim == linha || *fim != '\0' || convertido < INT_MIN || convertido > INT_MAX)
    puts("Entrada invalida. Digite um numero inteiro.");
else
    return (int) convertido;
```

`fim == linha` detecta "nenhum dígito foi lido"; `*fim != '\0'` detecta "tinha
número, mas seguido de letra" (`12abc`). Sem essa checagem, `scanf` +
`AdicionaFim`NumeroLidoMaisUm causaria um laço quase infinito de "opção inválida".

**`confirmar(const char *pergunta) -> int`** — lê a primeira letra e só aceita
`s`/`S`. Usada pela opção "criar novo arquivo limpo", que **avisa que a memória
será esvaziada** antes de apagar.

**`prepararDiretorio(void)`** — cria a pasta `dados/` com `_mkdir` (Windows,
`<direct.h>`) ou `mkdir` (POSIX, `<sys/stat.h>`), protegida por `#ifdef _WIN32`.
Isso é o que permite gerar o `.exe` em uma máquina e rodar em outra.

**`novoArquivoLimpo(caminho, limpar, gravar)`** — a função mais interessante do
`main.c` do ponto de vista de C, porque usa **ponteiro de função**:

```c
static void novoArquivoLimpo(const char *caminho, void (*limpar)(void),
                             int (*gravar)(void))
```

`void (*limpar)(void)` declara um parâmetro que é um **ponteiro para função**:
`limpar` não é a função em si, é o *endereço* dela. Assim a mesma rotina serve
para as quatro estruturas — basta passar o par certo:

```c
novoArquivoLimpo(LISTA_ARQUIVO, ListaLimpaLista, ListaGravarListaArquivo);
```

É o mesmo mecanismo de `qsort` da biblioteca padrão (`int (*comparar)(const void*, const void*)`).
A alternativa, sem ponteiros de função, seria repetir 15 linhas quatro vezes.

**`pausar(void)`** — só para a leitura humana: imprime "Pressione ENTER" e consome
a linha. (Se você canalizar a entrada de um arquivo para testar, lembre-se de
mandar uma linha em branco após cada comando, senão o `pausar` engole a linha
seguinte.)

---

## 10. Tabela de complexidades

| Operação | Lista simples | Lista dupla | Fila | Pilha |
|---|---|---|---|---|
| Inserir no **início** / frente | **O(1)** | **O(1)** | — | **O(1)** |
| Inserir no **fim** / traseira | O(n) | O(n) | **O(1)** | — |
| Remover do **início** / frente | **O(1)** | **O(1)** | **O(1)** | — |
| Remover do **fim** / traseira | O(n) | O(n) | — | **O(1)** |
| Remover por posição | O(n) | O(n) | — | — |
| Inserir ordenado | O(n) | O(n) | — | — |
| Buscar valor | O(n) | O(n) | O(n) | O(n) |
| Mostrar / Tamanho | O(n) | O(n) | O(n) | O(n) |
| Limpar | O(n) | O(n) | O(n) | O(n) |
| Gravar / Recuperar arquivo | O(n) | O(n) | O(n) | O(n) |

O único O(1) da lista dupla que a lista simples não tem é **navegar para
trás** — mas como não há função de busca para trás na interface, na prática as
duas têm o mesmo custo.

---

## 11. Armadilhas clássicas de prova

Checklist dos erros que mais aparecem neste tipo de trabalho:

1. **Ligar o ponteiro depois de liberar.** `free(atual)` antes de
   `atual = atual->prox` usa memória liberada.
2. **Usar o nó depois do `free`.** Guarde `int valor = no->valor;` antes.
3. **Não zerar a cabeça depois de liberar o último nó.** Deixa *use-after-free*.
4. **Confundir base 0 com base 1** em `RemovePosicao`.
5. **Não tratar a lista de 1 elemento** em `RemoveFim` (o laço
   `atual->prox->prox` estoura o `NULL`).
6. **Na lista dupla, esquecer de atualizar `ant` do sucessor** (ou o `prox` do
   antecessor). A lista continua funcionando "de frente", o que mascara o bug.
7. **Na lista dupla, não proteger o `if` ao mudar a cabeça** — `cabeca->ant = novo`
   com `cabeca == NULL` dá *segmentation fault*.
8. **`malloc` sem checar o retorno** e sem inicializar os campos.
9. **Usar o laço `atual != NULL` na fila circular** — nunca termina.
10. **Gravar ponteiros no arquivo** — só o conteúdo é serializável.
11. **Ler o arquivo e já limpar a lista antes de validar** — perde dados se o
    arquivo estiver ruim.
12. **Recuperar a pilha na ordem direta** — inverte a pilha.
13. **Duas funções com o mesmo nome em arquivos diferentes** — erro de link.
14. **`scanf` sem validar** — a entrada suja fica no buffer e desalinha o menu.

---

## 12. Autoavaliação

Tente responder sem olhar o código. Depois confira no final.

**1.** Em `ListaAdicionaInicio`, por que a ordem das duas linhas é obrigatória?
O que acontece se você inverter?

**2.** Explique, com um desenho, a diferença entre `while (atual->prox != NULL)` e
`while (atual->prox->prox != NULL)`. Qual função usa cada um e por quê?

**3.** Em `ListaRemovePosicao`, o que aconteceria se você removesse a linha
`anterior = atual;` de dentro do laço?

**4.** Por que `ListaLimpaLista` precisa terminar com `cabecaLista = NULL`, e o
que acontece com o programa se essa linha não existir?

**5.** Na fila, como se sabe que há exatamente 1 elemento? E por que o teste
`filaVazia()` da fila é `traseiraFila == NULL` e não `prox == NULL`?

**6.** Explique as três linhas de `FilaAdiciona` para o caso em que a fila já tem
elementos, desenhando o antes e o depois.

**7.** Por que `FilaMostraLista` não pode usar o laço `for (atual = frente();
atual != traseiraFila; ...)`?

**8.** Em `PilhaRecuperarListaArquivo`, por que o laço de reinserção vai de
`quantidade - 1` até `0`?

**9.** O que aconteceria, em termos de memória, se `RecuperarListaArquivo`
chamasse `LimpaLista()` logo depois do `fopen`, antes de validar a assinatura?

**10.** Qual é a finalidade da assinatura de 8 bytes no início do arquivo? E o que
acontece se o usuário renomear `fila.dat` para `lista_encadeada.dat`?

**11.** Explique o que são `extern No *cabecaLista;` (no `.h`) e
`No *cabecaLista = NULL;` (no `.c`), e o que aconteceria se o `extern` fosse
removido do `.h`.

**12.** Em `void (*limpar)(void)`, o que exatamente é `limpar`? O que
`ListaLimpaLista` (sem o `&`) e `&ListaLimpaLista` significam quando passados
para `novoArquivoLimpo`?

**13.** Por que `fgets` é mais seguro que `scanf("%d", &x)` para este menu?

**14.** Um aluno escreve `fwrite(&no->prox, sizeof(void *), 1, arquivo)` para
gravar a lista. Que dois problemas isso causa?

**15.** Se `malloc` falhar dentro de `criarNo` e a função devolver `NULL`, o que
acontece em `ListaAdicionaFim`? A lista fica consistente?

---

### Gabarito

**1.** `novo->prox = cabecaLista;` precisa acontecer **antes** de
`cabecaLista = novo;`, porque a segunda linha sobrescreve a cabeça. Invertendo,
`novo->prox` receberia o endereço do próprio `novo`, e a lista viraria um nó que
aponta para si mesmo (ciclo de 1 elemento, laço infinito em `MostraLista`).

**2.** O primeiro (`atual->prox != NULL`) caminha até o **último** nó: para quando
não há mais vizinho. É o laço de `AdicionaFim` (e de `RemovePosicao`, que
precisa achar o fim para validar o intervalo). O segundo
(`atual->prox->prox != NULL`) para no **penúltimo**, porque `atual->prox` é o nó
que ainda tem alguém depois dele — que é justamente o que se quer remover. É o
laço de `RemoveFim`.

**3.** `anterior` ficaria sempre `NULL`. Como o `if (anterior != NULL)` nunca seria
verdade, **toda** remoção cairia no `else` e faria `cabecaLista = atual->prox`,
destruindo a lista a cada chamada (os elementos não sumiriam, mas a cabeça
passaria a pular um nível por remoção).

**4.** Porque depois do `free` de todos os nós, `cabecaLista` guardaria o endereço
de memória já liberada. Qualquer operação seguinte (`ChecaLista`, `Tamanho`,
`Mostra`, um novo `AdicionaFim`) leria memória inválida: *use-after-free*,
comportamento imprevisível (normalmente um crash).

**5.** A fila tem 1 elemento quando o nó aponta para si mesmo:
`traseiraFila->prox == traseiraFila` (a frente e a traseira são o mesmo nó). O
vazio é `traseiraFila == NULL` porque, ao contrário das listas, aqui o ponteiro
guarda a **traseira** e a lista é circular: quando ela tem elementos, o `prox`
do último nunca é `NULL` — é a própria traseira.

**6.** Antes: `traseiraFila` = 30, e a sequência é `30 -> 100 -> 200 -> 30`
(10 é a frente). Depois de enfileirar 40: `40 -> 100 -> 200 -> 30 -> 40`, com
`traseiraFila` = 40. As três linhas: (1) `novo->prox = traseiraFila->prox` faz o
40 apontar para a frente (100); (2) `traseiraFila->prox = novo` religa o 30, que
era a traseira, ao 40; (3) `traseiraFila = novo` promove o 40 a nova traseira.
O efeito é inserir o novo nó entre a traseira antiga e a frente, fechando o
círculo.

**7.** Porque quando a fila tem 1 elemento a frente **é** a traseira, então a
condição `atual != traseiraFila` já é falsa na entrada e o corpo do laço nunca
executa — o único elemento não apareceria. O `for(;;)` com `if (atual ==
traseiraFila) break;` **depois** de imprimir resolve os dois casos.

**8.** Porque o arquivo guarda a pilha do topo para a base, e `PilhaAdiciona`
empilha por cima. Inserir na ordem direta faria cada novo elemento ser o topo,
invertendo a pilha (uma pilha com topo 10 viraria topo 30).

**9.** Se o arquivo não existisse, tivesse assinatura errada ou estivesse
truncado, a função retornaria 0 **depois** de já ter apagado a lista da memória.
O usuário perderia o trabalho sem ter recebido nenhum dado em troca. Por isso a
leitura é feita em duas fases (vetor temporário) e a `LimpaLista` só vem depois
do `fclose` bem-sucedido.

**10.** A assinatura identifica o **formato e a estrutura** do arquivo, para
recusar um arquivo de outra estrutura ou lixo qualquer. Se o usuário renomeasse
`fila.dat` para `lista_encadeada.dat`, a leitura compararia `"FILA0001"` com
`"LISTA001"`, encontraria diferença e responderia "Arquivo corrompido: assinatura
invalida" — em vez de carregar dados interpretados errado. É exatamente a
proteção que existe para isso.

**11.** O `extern` (no `.h`) é uma **declaração**: informa ao compilador que a
variável existe e será definida em outro arquivo, sem reservar memória. A linha do
`.c` é a **definição**, e é ela que reserva o espaço do ponteiro (8 bytes) e o
inicializa com `NULL`. Sem o `extern` no `.h`, o `main.c` receberia o erro
"implicit declaration" / "undefined reference" ao tentar usar `cabecaLista`.

**12.** `limpar` é um **ponteiro para função**: o endereço da função, não a
função. `ListaLimpaLista` é o nome da função, que ao ser usado como argumento
**decai para o seu endereço** (a única conversão automática de ponteiro-de-função
em C); `&ListaLimpaLista` é o mesmo endereço escrito explicitamente. Dentro de
`novoArquivoLimpo`, `limpar()` é a chamada indireta — é isso que permite a
mesma função atender as quatro estruturas.

**13.** `scanf("%d", &x)` deixa no buffer tudo que não coube no `int`. Se o
usuário digitar `abc`, o `scanf` falha e devolve o valor bufferizado, o menu
reaparece, e o próximo `scanf` consome `abc` de novo — o programa fica preso
repetindo "opção inválida". Com `fgets` + `strtol` + validação, a linha inteira é
consumida de uma vez e a entrada inválida é rejeitada com mensagem clara,
repedindo a pergunta.

**14.** Primeiro: **ponteiros não são portáveis entre programas nem entre
execuções** — o endereço de `no->prox` só faz sentido naquele processo, e ao
recuperar em outro ele apontaria para memória aleatória. Segundo: ao gravar os
`prox` de forma "crua", a ordem dos nós na memória não é a ordem da lista
(você não controla onde o `malloc` aloca), então a leitura não reconstruiria a
ordem correta. O formato correto grava **apenas os valores**, em ordem de
percurso, e reconstrói a lista com `malloc` do lado da leitura.

**15.** `criarNo` imprime o erro e devolve `NULL`; `ListaAdicionaFim` tem
`if (novo == NULL) return;`, então sai imediatamente **sem alterar a lista**.
Ela permanece consistente, apenas sem o elemento novo. (O mesmo `if` defensivo
existe em todas as funções de inserção das quatro estruturas.)
