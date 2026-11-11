/*
 * alugueres.c - Levantamentos, devolucoes e cancelamentos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "alugueres.h"
#include "bicicletas.h"
#include "utils.h"

/* Acrescenta um aluguer em curso ao vetor dinamico.
   Devolve o endereco do novo aluguer, ou NULL se faltar memoria. */
Aluguer *adicionarAluguer(Loja *loja, int idBike, int tipo, time_t inicio)
{
    Aluguer *novo = NULL;
    Aluguer *a = NULL;

    /* vetor cheio: pedir mais um bloco de posicoes */
    if (loja->numAlugueres == loja->capAlugueres) {
        novo = realloc(loja->alugueres,
                       (loja->capAlugueres + BLOCO_MEMORIA) * sizeof(Aluguer));
        if (novo == NULL)
            return NULL;
        loja->alugueres = novo;
        loja->capAlugueres += BLOCO_MEMORIA;
    }

    a = &loja->alugueres[loja->numAlugueres];
    /* limpar a estrutura toda (inclui os bytes de alinhamento que vao
       ser gravados no ficheiro binario) */
    memset(a, 0, sizeof(Aluguer));
    a->numero = loja->proximoNumAluguer;
    a->idBike = idBike;
    a->tipo = tipo;
    a->estado = ALUGUER_EM_CURSO;
    a->inicio = inicio;
    a->fim = 0;
    a->valor = 0.0f;
    loja->numAlugueres++;
    loja->proximoNumAluguer++;
    return a;
}

/* Procura um aluguer pelo numero; devolve NULL se nao existir */
Aluguer *procurarAluguer(Loja *loja, int numero)
{
    int i = 0;

    for (i = 0; i < loja->numAlugueres; i++) {
        if (loja->alugueres[i].numero == numero)
            return &loja->alugueres[i];
    }
    return NULL;
}

/* Numero de horas de utilizacao (H), arredondado sempre por excesso.
   Ex.: 1h05m conta como 2h. O minimo e HORAS_MINIMAS. */
int horasUtilizacao(time_t inicio, time_t fim)
{
    long segundos = (long)difftime(fim, inicio);
    int horas = (int)((segundos + SEGUNDOS_POR_HORA - 1) / SEGUNDOS_POR_HORA);

    if (horas < HORAS_MINIMAS)
        horas = HORAS_MINIMAS;
    return horas;
}

/* Regra de calculo do valor a pagar, para H horas de utilizacao:
   a) H <  LIMIAR_DIARIO: precoHora x H, com desconto se H >= 4;
   b) H >= LIMIAR_DIARIO: precoDia x D, com D = H/24 arredondado por excesso. */
float calcularValor(const Tarifa *tarifa, int horas)
{
    float valor = 0.0f;
    int dias = 0;

    if (horas < LIMIAR_DIARIO) {
        valor = tarifa->precoHora * horas;
        /* desconto de longa duracao */
        if (horas >= HORAS_MIN_DESCONTO)
            valor -= valor * DESCONTO_LONGA_DURACAO / PERCENTAGEM;
    } else {
        dias = (horas + HORAS_POR_DIA - 1) / HORAS_POR_DIA;
        valor = tarifa->precoDia * dias;
    }
    return valor;
}

/* Escreve o talao de um aluguer (no ecra ou num ficheiro) */
void imprimirTalao(const Aluguer *a, FILE *out)
{
    char data[TAM_DATA] = "";

    fprintf(out, "+-------------- BikeRent --------------+\n");
    fprintf(out, "  Aluguer n.    : %d\n", a->numero);
    fprintf(out, "  Bicicleta     : %d (%s)\n", a->idBike, nomeTipo(a->tipo));
    formatarData(a->inicio, data, TAM_DATA, 1);
    fprintf(out, "  Levantamento  : %s\n", data);

    /* o resto do talao depende do estado do aluguer */
    if (a->estado == ALUGUER_EM_CURSO) {
        fprintf(out, "  Estado        : Em curso\n");
    } else if (a->estado == ALUGUER_CANCELADO) {
        formatarData(a->fim, data, TAM_DATA, 1);
        fprintf(out, "  Estado        : Cancelado em %s\n", data);
    } else {
        formatarData(a->fim, data, TAM_DATA, 1);
        fprintf(out, "  Devolucao     : %s\n", data);
        fprintf(out, "  Horas cobradas: %d\n", horasUtilizacao(a->inicio, a->fim));
        fprintf(out, "  Valor a pagar : %.2f EUR\n", a->valor);
        fprintf(out, "  Estado        : Terminado\n");
    }
    fprintf(out, "+--------------------------------------+\n");
}

/* Pede o numero de um aluguer e devolve-o se estiver em curso;
   caso contrario mostra um aviso e devolve NULL */
static Aluguer *pedirAluguerEmCurso(Loja *loja)
{
    int numero = lerInteiro("Numero do aluguer: ", 1, INT_MAX);
    Aluguer *a = procurarAluguer(loja, numero);

    if (a == NULL) {
        printf("Nao existe nenhum aluguer com o numero %d.\n", numero);
        return NULL;
    }
    if (a->estado != ALUGUER_EM_CURSO) {
        printf("O aluguer %d ja nao esta em curso.\n", numero);
        return NULL;
    }
    return a;
}

/* Repoe como disponivel a bicicleta de um aluguer que terminou */
static void libertarBike(Loja *loja, const Aluguer *a)
{
    Bicicleta *b = procurarBike(loja, a->idBike);

    if (b != NULL) {
        b->estado = ESTADO_DISPONIVEL;
        b->dataEstado = a->fim;
    }
}

/* Funcionalidade 7: regista o levantamento de uma bicicleta.
   O utilizador escolhe o tipo e o sistema atribui a primeira disponivel. */
void registarLevantamento(Loja *loja)
{
    int tipo = TIPO_INVALIDO;
    int i = 0;
    time_t agora = time(NULL);
    time_t inicio = 0;
    Bicicleta *b = NULL;
    Aluguer *a = NULL;

    printf("\n--- Levantamento de bicicleta ---\n");
    tipo = lerTipo();

    /* procurar a primeira bicicleta disponivel do tipo escolhido */
    for (i = 0; i < loja->numBikes && b == NULL; i++) {
        if (loja->bikes[i].tipo == tipo && loja->bikes[i].estado == ESTADO_DISPONIVEL)
            b = &loja->bikes[i];
    }
    if (b == NULL) {
        printf("Nao ha bicicletas do tipo %s disponiveis.\n", nomeTipo(tipo));
        return;
    }

    /* o levantamento nao pode ser registado no futuro */
    do {
        inicio = lerMomento("Data/hora do levantamento");
        if (inicio > agora)
            printf("A data/hora do levantamento nao pode ser futura.\n");
    } while (inicio > agora);

    a = adicionarAluguer(loja, b->id, tipo, inicio);
    if (a == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }
    b->estado = ESTADO_ALUGADA;
    b->dataEstado = inicio;
    printf("\n");
    imprimirTalao(a, stdout);
}

/* Funcionalidade 8: regista a devolucao, calcula o valor e mostra o talao */
void registarDevolucao(Loja *loja)
{
    Aluguer *a = NULL;
    time_t fim = 0;

    printf("\n--- Devolucao de bicicleta ---\n");
    a = pedirAluguerEmCurso(loja);
    if (a == NULL)
        return;

    /* a devolucao nao pode ser anterior ao levantamento */
    do {
        fim = lerMomento("Data/hora da devolucao");
        if (fim < a->inicio)
            printf("A devolucao nao pode ser anterior ao levantamento.\n");
    } while (fim < a->inicio);

    a->fim = fim;
    a->valor = calcularValor(&loja->tarifas[a->tipo], horasUtilizacao(a->inicio, fim));
    a->estado = ALUGUER_TERMINADO;
    libertarBike(loja, a);
    printf("\n");
    imprimirTalao(a, stdout);
}

/* Funcionalidade 9: cancela um aluguer registado por engano.
   O aluguer fica marcado como cancelado e nao entra na faturacao. */
void cancelarAluguer(Loja *loja)
{
    Aluguer *a = NULL;

    printf("\n--- Cancelar aluguer ---\n");
    a = pedirAluguerEmCurso(loja);
    if (a == NULL)
        return;
    imprimirTalao(a, stdout);
    if (!confirmar("Confirma o cancelamento deste aluguer?"))
        return;
    a->estado = ALUGUER_CANCELADO;
    a->fim = time(NULL);
    a->valor = 0.0f;
    libertarBike(loja, a);
    printf("Aluguer %d cancelado; bicicleta %d novamente disponivel.\n",
           a->numero, a->idBike);
}
