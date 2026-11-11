/*
 * listagens.c - Listagens, tarifario e estatisticas.
 */
#include <stdio.h>
#include "listagens.h"
#include "bicicletas.h"
#include "alugueres.h"
#include "utils.h"

/* Devolve a descricao do estado de um aluguer */
static const char *nomeEstadoAluguer(char estado)
{
    switch (estado) {
    case ALUGUER_EM_CURSO:  return "Em curso";
    case ALUGUER_TERMINADO: return "Terminado";
    case ALUGUER_CANCELADO: return "Cancelado";
    default:                return "?";
    }
}

/* Escreve o cabecalho de uma listagem com um intervalo de datas */
static void imprimirIntervalo(FILE *out, const char *titulo, time_t ini, time_t fim)
{
    char d1[TAM_DATA] = "", d2[TAM_DATA] = "";

    formatarData(ini, d1, TAM_DATA, 0);
    formatarData(fim, d2, TAM_DATA, 0);
    fprintf(out, "\n=== %s (%s a %s) ===\n", titulo, d1, d2);
}

/* Escreve o cabecalho das colunas das listagens de alugueres */
static void cabecalhoAlugueres(FILE *out)
{
    fprintf(out, "%-6s %-5s %-9s %-17s %-17s %9s  %s\n",
            "N.", "Bike", "Tipo", "Levantamento", "Devolucao", "Valor", "Estado");
}

/* Escreve uma linha com os dados de um aluguer */
static void linhaAluguer(const Aluguer *a, FILE *out)
{
    char d1[TAM_DATA] = "", d2[TAM_DATA] = "";

    formatarData(a->inicio, d1, TAM_DATA, 1);
    formatarData(a->fim, d2, TAM_DATA, 1);
    fprintf(out, "%-6d %-5d %-9s %-17s %-17s %9.2f  %s\n",
            a->numero, a->idBike, nomeTipo(a->tipo), d1, d2, a->valor,
            nomeEstadoAluguer(a->estado));
}

/* Funcionalidade 12: lista todas as bicicletas e o estado, com paginacao */
void listarBikes(const Loja *loja, FILE *out)
{
    char dEstado[TAM_DATA] = "", dReg[TAM_DATA] = "", dTerm[TAM_DATA] = "";
    const Bicicleta *b = NULL;
    int i = 0;
    int continuar = 1;

    fprintf(out, "\n=== Bicicletas (%d) ===\n", loja->numBikes);
    fprintf(out, "%-5s %-9s %-11s %-17s %-11s %-11s %s\n",
            "N.", "Tipo", "Estado", "Desde", "Registo", "Terminus", "Motivo");

    /* percorrer a frota; parar se o utilizador sair da paginacao */
    for (i = 0; i < loja->numBikes && continuar; i++) {
        b = &loja->bikes[i];
        formatarData(b->dataEstado, dEstado, TAM_DATA, 1);
        formatarData(b->dataRegisto, dReg, TAM_DATA, 0);
        formatarData(b->dataTerminus, dTerm, TAM_DATA, 0);
        fprintf(out, "%-5d %-9s %-11s %-17s %-11s %-11s %s\n",
                b->id, nomeTipo(b->tipo), nomeEstado(b->estado),
                dEstado, dReg, dTerm, nomeMotivo(b->motivo));
        /* so pergunta se ainda houver mais bicicletas para mostrar */
        if (i + 1 < loja->numBikes)
            continuar = continuarPagina(out, i + 1);
    }
}

/* Funcionalidade 13: lista os alugueres iniciados entre duas datas */
void listarAlugueresIntervalo(const Loja *loja, FILE *out, time_t ini, time_t fim)
{
    const Aluguer *a = NULL;
    int i = 0, total = 0;
    int continuar = 1;

    imprimirIntervalo(out, "Alugueres realizados", ini, fim);
    cabecalhoAlugueres(out);

    /* mostrar os alugueres cujo levantamento caia no intervalo */
    for (i = 0; i < loja->numAlugueres && continuar; i++) {
        a = &loja->alugueres[i];
        if (a->inicio >= ini && a->inicio <= fim) {
            linhaAluguer(a, out);
            total++;
            continuar = continuarPagina(out, total);
        }
    }
    fprintf(out, "Total: %d aluguer(es).\n", total);
}

/* Funcionalidade 14: lista os alugueres em curso (bicicletas por devolver) */
void listarAlugueresEmCurso(const Loja *loja, FILE *out)
{
    const Aluguer *a = NULL;
    int i = 0, total = 0;
    int continuar = 1;

    fprintf(out, "\n=== Alugueres em curso ===\n");
    cabecalhoAlugueres(out);

    for (i = 0; i < loja->numAlugueres && continuar; i++) {
        a = &loja->alugueres[i];
        if (a->estado == ALUGUER_EM_CURSO) {
            linhaAluguer(a, out);
            total++;
            continuar = continuarPagina(out, total);
        }
    }
    fprintf(out, "Total: %d aluguer(es) em curso.\n", total);
}

/* Mostra o tarifario e os parametros da regra de precos */
void mostrarTarifario(const Loja *loja, FILE *out)
{
    int i = 0;

    fprintf(out, "\n=== Tarifario ===\n");
    fprintf(out, "%-9s %10s %10s\n", "Tipo", "Preco/hora", "Preco/dia");
    for (i = 0; i < NUM_TIPOS; i++)
        fprintf(out, "%-9s %10.2f %10.2f\n", nomeTipo(i),
                loja->tarifas[i].precoHora, loja->tarifas[i].precoDia);
    fprintf(out, "Preco ao dia a partir de %d horas; desconto de %d%% a partir "
            "de %d horas.\n", LIMIAR_DIARIO, DESCONTO_LONGA_DURACAO,
            HORAS_MIN_DESCONTO);
}

/* Funcionalidade 15: valor total faturado entre duas datas.
   Conta os alugueres terminados cuja devolucao caia no intervalo. */
void mostrarFaturacao(const Loja *loja, FILE *out, time_t ini, time_t fim)
{
    const Aluguer *a = NULL;
    float total = 0.0f;
    int i = 0, quantos = 0;

    for (i = 0; i < loja->numAlugueres; i++) {
        a = &loja->alugueres[i];
        if (a->estado == ALUGUER_TERMINADO && a->fim >= ini && a->fim <= fim) {
            total += a->valor;
            quantos++;
        }
    }
    imprimirIntervalo(out, "Faturacao", ini, fim);
    fprintf(out, "Alugueres pagos : %d\n", quantos);
    fprintf(out, "Total faturado  : %.2f EUR\n", total);
}

/* Funcionalidade 16: tipo de bicicleta mais alugado num intervalo.
   Conta os alugueres nao cancelados iniciados no intervalo. */
void mostrarTipoMaisAlugado(const Loja *loja, FILE *out, time_t ini, time_t fim)
{
    const Aluguer *a = NULL;
    int contagem[NUM_TIPOS] = {0};  /* numero de alugueres de cada tipo */
    int i = 0, maior = 0;

    for (i = 0; i < loja->numAlugueres; i++) {
        a = &loja->alugueres[i];
        if (a->estado != ALUGUER_CANCELADO && a->inicio >= ini && a->inicio <= fim)
            contagem[a->tipo]++;
    }

    imprimirIntervalo(out, "Alugueres por tipo", ini, fim);
    /* mostrar a contagem de cada tipo e descobrir o maximo */
    for (i = 0; i < NUM_TIPOS; i++) {
        fprintf(out, "%-9s : %d\n", nomeTipo(i), contagem[i]);
        if (contagem[i] > contagem[maior])
            maior = i;
    }

    if (contagem[maior] == 0) {
        fprintf(out, "Nao ha alugueres neste intervalo.\n");
        return;
    }
    /* em caso de empate sao mostrados todos os tipos com o maximo */
    fprintf(out, "Tipo(s) mais alugado(s):");
    for (i = 0; i < NUM_TIPOS; i++) {
        if (contagem[i] == contagem[maior])
            fprintf(out, " %s", nomeTipo(i));
    }
    fprintf(out, " (%d aluguer(es))\n", contagem[maior]);
}

/* Funcionalidade 18: duracao media, em minutos, dos alugueres terminados
   (geral e por tipo de bicicleta) */
void mostrarDuracaoMedia(const Loja *loja, FILE *out)
{
    const Aluguer *a = NULL;
    double soma[NUM_TIPOS] = {0.0};  /* minutos acumulados por tipo   */
    int quantos[NUM_TIPOS] = {0};    /* alugueres terminados por tipo */
    double somaGeral = 0.0;
    int quantosGeral = 0;
    int i = 0;

    for (i = 0; i < loja->numAlugueres; i++) {
        a = &loja->alugueres[i];
        if (a->estado == ALUGUER_TERMINADO) {
            soma[a->tipo] += difftime(a->fim, a->inicio) / SEGUNDOS_POR_MINUTO;
            quantos[a->tipo]++;
        }
    }

    fprintf(out, "\n=== Duracao media dos alugueres terminados (minutos) ===\n");
    for (i = 0; i < NUM_TIPOS; i++) {
        somaGeral += soma[i];
        quantosGeral += quantos[i];
        /* evitar a divisao por zero quando nao ha alugueres do tipo */
        if (quantos[i] > 0)
            fprintf(out, "%-9s : %.1f min (%d aluguer(es))\n",
                    nomeTipo(i), soma[i] / quantos[i], quantos[i]);
        else
            fprintf(out, "%-9s : sem alugueres terminados\n", nomeTipo(i));
    }
    if (quantosGeral > 0)
        fprintf(out, "%-9s : %.1f min (%d aluguer(es))\n",
                "GERAL", somaGeral / quantosGeral, quantosGeral);
    else
        fprintf(out, "%-9s : sem alugueres terminados\n", "GERAL");
}

/* Gestao de precos: altera o preco/hora e o preco/dia de um tipo */
void alterarPrecos(Loja *loja)
{
    int tipo = TIPO_INVALIDO;

    printf("\n--- Alterar precos ---\n");
    mostrarTarifario(loja, stdout);
    tipo = lerTipo();
    loja->tarifas[tipo].precoHora = lerFloat("Novo preco por hora: ", 0.0f, PRECO_MAXIMO);
    loja->tarifas[tipo].precoDia = lerFloat("Novo preco por dia: ", 0.0f, PRECO_MAXIMO);
    printf("Precos do tipo %s atualizados.\n", nomeTipo(tipo));
}
