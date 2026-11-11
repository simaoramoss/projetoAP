/*
 * ficheiros.c - Instalacao (ficheiros de texto) e persistencia (binarios).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ficheiros.h"
#include "bicicletas.h"
#include "alugueres.h"

/* Devolve 1 se ja existirem dados gravados de uma sessao anterior */
int existemDadosGravados(void)
{
    FILE *f = fopen(FICH_BIKES_BIN, "rb");

    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}

/* Calcula a data de fim de vida por omissao (registo + VIDA_UTIL_ANOS) */
static time_t terminusPorOmissao(time_t registo)
{
    struct tm t = *localtime(&registo);

    t.tm_year += VIDA_UTIL_ANOS;
    return mktime(&t);
}

/* Funcionalidade 1: le a frota inicial do ficheiro de texto.
   Linhas que nao sejam "<id> <tipo> <estado>" validos (cabecalho, linhas
   em branco) sao ignoradas. Devolve 0 em caso de erro. */
static int lerFrotaInicial(Loja *loja)
{
    FILE *f = fopen(FICH_BIKES_TXT, "r");
    char linha[MAX_LINHA] = "";
    char nome[MAX_NOME_TIPO] = "";
    char estado = '\0';
    int id = 0, tipo = TIPO_INVALIDO;
    int ignoradas = 0;      /* bicicletas que ja nao cabem na capacidade */
    time_t agora = time(NULL);
    Bicicleta *b = NULL;

    if (f == NULL) {
        printf("Erro: nao foi possivel abrir '%s'.\n", FICH_BIKES_TXT);
        return 0;
    }

    /* ler o ficheiro linha a linha ate ao fim */
    while (fgets(linha, MAX_LINHA, f) != NULL) {
        /* saltar linhas que nao tenham os 3 campos (cabecalho, vazias) */
        if (sscanf(linha, "%d %19s %c", &id, nome, &estado) != 3)
            continue;
        tipo = tipoPorNome(nome);
        /* saltar linhas com dados invalidos ou ids repetidos */
        if (id <= 0 || tipo == TIPO_INVALIDO
            || strchr(ESTADOS_VALIDOS, estado) == NULL
            || procurarBike(loja, id) != NULL) {
            printf("Aviso: linha ignorada por ser invalida: %s", linha);
            continue;
        }
        /* validar a capacidade (as terminadas nao ocupam lugar) */
        if (estado != ESTADO_TERMINADA && contarAtivas(loja) >= CAPACIDADE_MAX) {
            ignoradas++;
            continue;
        }

        b = adicionarBike(loja, id, tipo, estado, agora, terminusPorOmissao(agora));
        if (b == NULL) {
            printf("Erro: memoria insuficiente.\n");
            fclose(f);
            return 0;
        }
        /* bicicletas ja alugadas: criar o aluguer com inicio na instalacao */
        if (estado == ESTADO_ALUGADA && adicionarAluguer(loja, id, tipo, agora) == NULL) {
            printf("Erro: memoria insuficiente.\n");
            fclose(f);
            return 0;
        }
    }
    fclose(f);

    if (ignoradas > 0)
        printf("Aviso: a frota inicial excede a CAPACIDADE_MAX (%d); "
               "%d bicicletas nao foram carregadas.\n", CAPACIDADE_MAX, ignoradas);
    return 1;
}

/* Funcionalidade 2: le o tarifario do ficheiro de texto.
   Devolve 0 se o ficheiro nao abrir ou faltar o preco de algum tipo. */
static int lerTarifarioInicial(Loja *loja)
{
    FILE *f = fopen(FICH_TARIFARIO_TXT, "r");
    char linha[MAX_LINHA] = "";
    char nome[MAX_NOME_TIPO] = "";
    float hora = 0.0f, dia = 0.0f;
    int tipo = TIPO_INVALIDO;
    int lido[NUM_TIPOS] = {0};  /* 1 se o preco desse tipo ja foi lido */
    int i = 0;

    if (f == NULL) {
        printf("Erro: nao foi possivel abrir '%s'.\n", FICH_TARIFARIO_TXT);
        return 0;
    }

    /* ler o ficheiro linha a linha ate ao fim */
    while (fgets(linha, MAX_LINHA, f) != NULL) {
        /* saltar o cabecalho e linhas incompletas */
        if (sscanf(linha, "%19s %f %f", nome, &hora, &dia) != 3)
            continue;
        tipo = tipoPorNome(nome);
        if (tipo == TIPO_INVALIDO || hora < 0.0f || dia < 0.0f) {
            printf("Aviso: linha do tarifario ignorada: %s", linha);
            continue;
        }
        loja->tarifas[tipo].precoHora = hora;
        loja->tarifas[tipo].precoDia = dia;
        lido[tipo] = 1;
    }
    fclose(f);

    /* confirmar que todos os tipos ficaram com preco */
    for (i = 0; i < NUM_TIPOS; i++) {
        if (!lido[i]) {
            printf("Erro: falta o preco do tipo %s no tarifario.\n", nomeTipo(i));
            return 0;
        }
    }
    return 1;
}

/* Instalacao: carrega a frota e o tarifario a partir dos ficheiros .txt */
int instalar(Loja *loja)
{
    printf("Primeira utilizacao: a instalar a partir dos ficheiros de texto...\n");
    if (!lerFrotaInicial(loja) || !lerTarifarioInicial(loja))
        return 0;
    printf("Instalacao concluida: %d bicicletas carregadas.\n", loja->numBikes);
    return 1;
}

/* Funcionalidade 3: carrega os dados gravados na sessao anterior.
   Devolve 0 em caso de erro. */
int carregarDados(Loja *loja)
{
    FILE *f = NULL;
    int ok = 1;

    /* --- bicicletas: contador, proximo id e vetor --- */
    f = fopen(FICH_BIKES_BIN, "rb");
    if (f == NULL)
        return 0;
    ok = fread(&loja->numBikes, sizeof(int), 1, f) == 1
         && fread(&loja->proximoIdBike, sizeof(int), 1, f) == 1
         && loja->numBikes >= 0;
    if (ok) {
        /* reservar espaco para as bicicletas lidas e mais um bloco livre */
        loja->capBikes = loja->numBikes + BLOCO_MEMORIA;
        loja->bikes = malloc(loja->capBikes * sizeof(Bicicleta));
        ok = loja->bikes != NULL
             && fread(loja->bikes, sizeof(Bicicleta), loja->numBikes, f)
                == (size_t)loja->numBikes;
    }
    fclose(f);
    if (!ok)
        return 0;

    /* --- alugueres: contador, proximo numero e vetor --- */
    f = fopen(FICH_ALUGUERES_BIN, "rb");
    if (f == NULL)
        return 0;
    ok = fread(&loja->numAlugueres, sizeof(int), 1, f) == 1
         && fread(&loja->proximoNumAluguer, sizeof(int), 1, f) == 1
         && loja->numAlugueres >= 0;
    if (ok) {
        loja->capAlugueres = loja->numAlugueres + BLOCO_MEMORIA;
        loja->alugueres = malloc(loja->capAlugueres * sizeof(Aluguer));
        ok = loja->alugueres != NULL
             && fread(loja->alugueres, sizeof(Aluguer), loja->numAlugueres, f)
                == (size_t)loja->numAlugueres;
    }
    fclose(f);
    if (!ok)
        return 0;

    /* --- tarifario --- */
    f = fopen(FICH_TARIFARIO_BIN, "rb");
    if (f == NULL)
        return 0;
    ok = fread(loja->tarifas, sizeof(Tarifa), NUM_TIPOS, f) == NUM_TIPOS;
    fclose(f);
    return ok;
}

/* Funcionalidade 20: grava todos os dados em ficheiros binarios.
   Devolve 0 em caso de erro. */
int gravarDados(const Loja *loja)
{
    FILE *f = NULL;
    int ok = 1;

    /* --- bicicletas --- */
    f = fopen(FICH_BIKES_BIN, "wb");
    if (f == NULL)
        return 0;
    ok = fwrite(&loja->numBikes, sizeof(int), 1, f) == 1
         && fwrite(&loja->proximoIdBike, sizeof(int), 1, f) == 1
         && fwrite(loja->bikes, sizeof(Bicicleta), loja->numBikes, f)
            == (size_t)loja->numBikes;
    /* o fclose tambem pode falhar (ex.: disco cheio) */
    if (fclose(f) != 0 || !ok)
        return 0;

    /* --- alugueres --- */
    f = fopen(FICH_ALUGUERES_BIN, "wb");
    if (f == NULL)
        return 0;
    ok = fwrite(&loja->numAlugueres, sizeof(int), 1, f) == 1
         && fwrite(&loja->proximoNumAluguer, sizeof(int), 1, f) == 1
         && fwrite(loja->alugueres, sizeof(Aluguer), loja->numAlugueres, f)
            == (size_t)loja->numAlugueres;
    if (fclose(f) != 0 || !ok)
        return 0;

    /* --- tarifario --- */
    f = fopen(FICH_TARIFARIO_BIN, "wb");
    if (f == NULL)
        return 0;
    ok = fwrite(loja->tarifas, sizeof(Tarifa), NUM_TIPOS, f) == NUM_TIPOS;
    if (fclose(f) != 0 || !ok)
        return 0;
    return 1;
}
