/*
 * bicicletas.c - Gestao da frota de bicicletas.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "bicicletas.h"
#include "utils.h"

/* Coloca a loja vazia (sem memoria reservada) */
void iniciarLoja(Loja *loja)
{
    int i = 0;

    loja->bikes = NULL;
    loja->numBikes = 0;
    loja->capBikes = 0;
    loja->proximoIdBike = 1;
    loja->alugueres = NULL;
    loja->numAlugueres = 0;
    loja->capAlugueres = 0;
    loja->proximoNumAluguer = 1;
    /* tarifario a zeros ate ser lido de ficheiro */
    for (i = 0; i < NUM_TIPOS; i++) {
        loja->tarifas[i].precoHora = 0.0f;
        loja->tarifas[i].precoDia = 0.0f;
    }
}

/* Liberta a memoria dinamica da loja */
void libertarLoja(Loja *loja)
{
    free(loja->bikes);
    free(loja->alugueres);
    iniciarLoja(loja);
}

/* Devolve o nome de um tipo de bicicleta */
const char *nomeTipo(int tipo)
{
    switch (tipo) {
    case TIPO_BTT:      return "BTT";
    case TIPO_ESTRADA:  return "ESTRADA";
    case TIPO_ELETRICA: return "ELETRICA";
    case TIPO_INFANTIL: return "INFANTIL";
    default:            return "?";
    }
}

/* Devolve o tipo com o nome dado, ou TIPO_INVALIDO se nao existir */
int tipoPorNome(const char *nome)
{
    int i = 0;

    /* comparar o nome com o de cada tipo conhecido */
    for (i = 0; i < NUM_TIPOS; i++) {
        if (strcmp(nome, nomeTipo(i)) == 0)
            return i;
    }
    return TIPO_INVALIDO;
}

/* Devolve a descricao de um estado de bicicleta */
const char *nomeEstado(char estado)
{
    switch (estado) {
    case ESTADO_NOVA:       return "Nova";
    case ESTADO_DISPONIVEL: return "Disponivel";
    case ESTADO_ALUGADA:    return "Alugada";
    case ESTADO_MANUTENCAO: return "Manutencao";
    case ESTADO_TERMINADA:  return "Terminada";
    default:                return "?";
    }
}

/* Devolve a descricao de um motivo de manutencao */
const char *nomeMotivo(char motivo)
{
    switch (motivo) {
    case MOTIVO_AVARIA:  return "Avaria";
    case MOTIVO_REVISAO: return "Revisao";
    case MOTIVO_OUTRO:   return "Outro";
    default:             return "-";
    }
}

/* Mostra os tipos de bicicleta e pede ao utilizador que escolha um */
int lerTipo(void)
{
    int i = 0;

    /* mostrar os tipos numerados a partir de 1 */
    for (i = 0; i < NUM_TIPOS; i++)
        printf("  %d - %s\n", i + 1, nomeTipo(i));
    return lerInteiro("Tipo de bicicleta: ", 1, NUM_TIPOS) - 1;
}

/* Conta as bicicletas que estao num dado estado */
int contarEstado(const Loja *loja, char estado)
{
    int i = 0, total = 0;

    for (i = 0; i < loja->numBikes; i++) {
        if (loja->bikes[i].estado == estado)
            total++;
    }
    return total;
}

/* Conta as bicicletas nao terminadas (as que ocupam capacidade) */
int contarAtivas(const Loja *loja)
{
    return loja->numBikes - contarEstado(loja, ESTADO_TERMINADA);
}

/* Procura uma bicicleta pelo id; devolve NULL se nao existir */
Bicicleta *procurarBike(Loja *loja, int id)
{
    int i = 0;

    for (i = 0; i < loja->numBikes; i++) {
        if (loja->bikes[i].id == id)
            return &loja->bikes[i];
    }
    return NULL;
}

/* Acrescenta uma bicicleta ao vetor dinamico (aumentando-o se necessario).
   Devolve o endereco da nova bicicleta, ou NULL se faltar memoria. */
Bicicleta *adicionarBike(Loja *loja, int id, int tipo, char estado,
                         time_t registo, time_t terminus)
{
    Bicicleta *novo = NULL;
    Bicicleta *b = NULL;

    /* vetor cheio: pedir mais um bloco de posicoes */
    if (loja->numBikes == loja->capBikes) {
        novo = realloc(loja->bikes,
                       (loja->capBikes + BLOCO_MEMORIA) * sizeof(Bicicleta));
        if (novo == NULL)
            return NULL;
        loja->bikes = novo;
        loja->capBikes += BLOCO_MEMORIA;
    }

    b = &loja->bikes[loja->numBikes];
    /* limpar a estrutura toda (inclui os bytes de alinhamento que vao
       ser gravados no ficheiro binario) */
    memset(b, 0, sizeof(Bicicleta));
    b->id = id;
    b->tipo = tipo;
    b->estado = estado;
    /* a frota inicial nao indica o motivo: assume-se "outro" */
    b->motivo = (estado == ESTADO_MANUTENCAO) ? MOTIVO_OUTRO : MOTIVO_NENHUM;
    b->dataRegisto = registo;
    b->dataTerminus = terminus;
    b->dataEstado = registo;
    loja->numBikes++;

    /* garantir que o proximo id gerado nunca repete um existente */
    if (id >= loja->proximoIdBike)
        loja->proximoIdBike = id + 1;
    return b;
}

/* Pede o id de uma bicicleta e devolve-a; NULL (com aviso) se nao existir */
static Bicicleta *pedirBike(Loja *loja)
{
    int id = lerInteiro("Numero da bicicleta: ", 1, INT_MAX);
    Bicicleta *b = procurarBike(loja, id);

    if (b == NULL)
        printf("Nao existe nenhuma bicicleta com o numero %d.\n", id);
    return b;
}

/* Funcionalidade 5: regista uma nova bicicleta (nova aquisicao) */
void registarNovaBike(Loja *loja)
{
    int tipo = TIPO_INVALIDO;
    time_t agora = time(NULL);
    time_t terminus = 0;
    Bicicleta *b = NULL;

    printf("\n--- Registar nova bicicleta ---\n");
    /* as bicicletas terminadas nao contam para a capacidade */
    if (contarAtivas(loja) >= CAPACIDADE_MAX) {
        printf("Capacidade maxima atingida (%d bicicletas).\n", CAPACIDADE_MAX);
        return;
    }

    tipo = lerTipo();
    /* a data de fim de vida tem de ser posterior ao registo */
    do {
        terminus = lerDia("Data de terminus de operacao/vida", 1);
        if (terminus <= agora)
            printf("A data de terminus tem de ser futura.\n");
    } while (terminus <= agora);

    b = adicionarBike(loja, loja->proximoIdBike, tipo, ESTADO_NOVA, agora, terminus);
    if (b == NULL)
        printf("Erro: memoria insuficiente.\n");
    else
        printf("Bicicleta %d (%s) registada no estado NOVA.\n", b->id, nomeTipo(tipo));
}

/* Passa uma bicicleta NOVA a DISPONIVEL, para poder ser alugada */
void colocarEmServico(Loja *loja)
{
    Bicicleta *b = NULL;

    printf("\n--- Colocar bicicleta nova em servico ---\n");
    b = pedirBike(loja);
    if (b == NULL)
        return;
    if (b->estado != ESTADO_NOVA) {
        printf("A bicicleta %d nao esta no estado NOVA (%s).\n",
               b->id, nomeEstado(b->estado));
        return;
    }
    b->estado = ESTADO_DISPONIVEL;
    b->dataEstado = time(NULL);
    printf("Bicicleta %d passou a DISPONIVEL.\n", b->id);
}

/* Funcionalidade 6: elimina uma bicicleta (fim de vida).
   O registo mantem-se, no estado TERMINADA, para preservar o historico. */
void eliminarBike(Loja *loja)
{
    Bicicleta *b = NULL;

    printf("\n--- Eliminar bicicleta ---\n");
    b = pedirBike(loja);
    if (b == NULL)
        return;
    /* uma bicicleta alugada tem de ser devolvida primeiro */
    if (b->estado == ESTADO_ALUGADA) {
        printf("A bicicleta %d esta alugada; registe primeiro a devolucao.\n", b->id);
        return;
    }
    if (b->estado == ESTADO_TERMINADA) {
        printf("A bicicleta %d ja esta terminada.\n", b->id);
        return;
    }
    if (!confirmar("Confirma a eliminacao desta bicicleta?"))
        return;
    b->estado = ESTADO_TERMINADA;
    b->motivo = MOTIVO_NENHUM;
    b->dataEstado = time(NULL);
    printf("Bicicleta %d eliminada (estado TERMINADA).\n", b->id);
}

/* Funcionalidade 10: marca uma bicicleta para manutencao, com motivo */
void marcarManutencao(Loja *loja)
{
    Bicicleta *b = NULL;

    printf("\n--- Marcar bicicleta para manutencao ---\n");
    b = pedirBike(loja);
    if (b == NULL)
        return;
    /* so bicicletas que estao na loja (novas ou disponiveis) podem ir */
    if (b->estado != ESTADO_DISPONIVEL && b->estado != ESTADO_NOVA) {
        printf("A bicicleta %d nao pode ir para manutencao (estado: %s).\n",
               b->id, nomeEstado(b->estado));
        return;
    }
    b->motivo = lerOpcaoChar("Motivo - (a)varia, (r)evisao periodica, (o)utro: ",
                             MOTIVOS_VALIDOS);
    b->estado = ESTADO_MANUTENCAO;
    b->dataEstado = time(NULL);
    printf("Bicicleta %d em MANUTENCAO (%s).\n", b->id, nomeMotivo(b->motivo));
}

/* Funcionalidade 10: reverte a marcacao de manutencao */
void reverterManutencao(Loja *loja)
{
    Bicicleta *b = NULL;

    printf("\n--- Terminar/reverter manutencao ---\n");
    b = pedirBike(loja);
    if (b == NULL)
        return;
    if (b->estado != ESTADO_MANUTENCAO) {
        printf("A bicicleta %d nao esta em manutencao.\n", b->id);
        return;
    }
    b->estado = ESTADO_DISPONIVEL;
    b->motivo = MOTIVO_NENHUM;
    b->dataEstado = time(NULL);
    printf("Bicicleta %d passou a DISPONIVEL.\n", b->id);
}
