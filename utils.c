/*
 * utils.c - Leitura validada de dados, datas e apoio as listagens.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "constantes.h"
#include "utils.h"

/* Le uma linha do teclado para buf (sem o '\n').
   Se a linha for maior que o buffer, o resto e descartado. */
void lerLinha(char *buf, int tam)
{
    int c = 0;
    size_t n = 0;

    /* se a entrada terminou nao e possivel continuar a pedir dados */
    if (fgets(buf, tam, stdin) == NULL) {
        printf("\nFim da entrada de dados. A terminar.\n");
        exit(EXIT_FAILURE);
    }
    n = strlen(buf);
    /* linha completa: retirar o '\n'; incompleta: limpar o resto */
    if (n > 0 && buf[n - 1] == '\n') {
        buf[n - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
}

/* Pede um inteiro entre min e max (inclusive), repetindo ate ser valido */
int lerInteiro(const char *msg, int min, int max)
{
    char linha[MAX_LINHA] = "";
    char *fim = NULL;
    long valor = 0;
    int valido = 0;

    /* repetir ate o utilizador escrever um inteiro dentro dos limites */
    do {
        printf("%s", msg);
        lerLinha(linha, MAX_LINHA);
        valor = strtol(linha, &fim, 10);
        /* valido se leu algum digito, nao sobrou texto e esta nos limites */
        valido = (fim != linha && *fim == '\0' && valor >= min && valor <= max);
        if (!valido)
            printf("Valor invalido (deve ser um inteiro entre %d e %d).\n", min, max);
    } while (!valido);
    return (int)valor;
}

/* Pede um numero real entre min e max (inclusive); aceita ponto ou virgula */
float lerFloat(const char *msg, float min, float max)
{
    char linha[MAX_LINHA] = "";
    char *fim = NULL;
    char *virgula = NULL;
    float valor = 0.0f;
    int valido = 0;

    /* repetir ate o utilizador escrever um numero dentro dos limites */
    do {
        printf("%s", msg);
        lerLinha(linha, MAX_LINHA);
        /* trocar a virgula decimal por ponto, que e o que o strtod espera */
        virgula = strchr(linha, ',');
        if (virgula != NULL)
            *virgula = '.';
        valor = (float)strtod(linha, &fim);
        valido = (fim != linha && *fim == '\0' && valor >= min && valor <= max);
        if (!valido)
            printf("Valor invalido (deve estar entre %.2f e %.2f).\n", min, max);
    } while (!valido);
    return valor;
}

/* Pede uma letra que pertenca a string validos (em minusculas) */
char lerOpcaoChar(const char *msg, const char *validos)
{
    char linha[MAX_LINHA] = "";
    char c = '\0';
    int valido = 0;

    /* repetir ate ser escrita exatamente uma das letras validas */
    do {
        printf("%s", msg);
        lerLinha(linha, MAX_LINHA);
        c = (char)tolower((unsigned char)linha[0]);
        valido = (c != '\0' && linha[1] == '\0' && strchr(validos, c) != NULL);
        if (!valido)
            printf("Opcao invalida (opcoes: %s).\n", validos);
    } while (!valido);
    return c;
}

/* Faz uma pergunta de sim/nao; devolve 1 se a resposta for 's' */
int confirmar(const char *msg)
{
    char pergunta[MAX_LINHA] = "";

    snprintf(pergunta, MAX_LINHA, "%s (s/n): ", msg);
    return lerOpcaoChar(pergunta, "sn") == 's';
}

/* Espera que o utilizador prima Enter */
void pausa(void)
{
    char linha[MAX_LINHA] = "";

    printf("\nPrima Enter para continuar...");
    lerLinha(linha, MAX_LINHA);
}

/* Devolve 1 se o ano for bissexto */
static int bissexto(int ano)
{
    return (ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0;
}

/* Devolve 1 se dia/mes/ano formarem uma data valida */
static int dataValida(int dia, int mes, int ano)
{
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (ano < ANO_MINIMO || ano > ANO_MAXIMO || mes < 1 || mes > 12)
        return 0;
    /* fevereiro tem mais um dia nos anos bissextos */
    if (bissexto(ano))
        diasMes[1] = 29;
    return dia >= 1 && dia <= diasMes[mes - 1];
}

/* Converte uma data e hora (ja validadas) em time_t */
static time_t criarTempo(int dia, int mes, int ano, int hora, int min, int seg)
{
    struct tm t = {0};

    t.tm_mday = dia;
    t.tm_mon = mes - 1;              /* struct tm conta os meses de 0 a 11 */
    t.tm_year = ano - ANO_BASE_TM;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;                 /* o sistema decide a hora de verao   */
    return mktime(&t);
}

/* Pede um dia (dd/mm/aaaa). Se fimDoDia != 0 devolve as 23:59:59 desse dia,
   caso contrario as 00:00:00 (util para intervalos de datas). */
time_t lerDia(const char *msg, int fimDoDia)
{
    char linha[MAX_LINHA] = "";
    char lixo = '\0';
    int dia = 0, mes = 0, ano = 0;
    int valido = 0;

    /* repetir ate ser escrita uma data valida */
    do {
        printf("%s (dd/mm/aaaa): ", msg);
        lerLinha(linha, MAX_LINHA);
        valido = (sscanf(linha, "%d/%d/%d %c", &dia, &mes, &ano, &lixo) == 3
                  && dataValida(dia, mes, ano));
        if (!valido)
            printf("Data invalida.\n");
    } while (!valido);

    if (fimDoDia)
        return criarTempo(dia, mes, ano, 23, 59, 59);
    return criarTempo(dia, mes, ano, 0, 0, 0);
}

/* Pede uma data e hora (dd/mm/aaaa hh:mm) */
time_t lerDataHora(const char *msg)
{
    char linha[MAX_LINHA] = "";
    char lixo = '\0';
    int dia = 0, mes = 0, ano = 0, hora = 0, min = 0;
    int valido = 0;

    /* repetir ate ser escrita uma data/hora valida */
    do {
        printf("%s (dd/mm/aaaa hh:mm): ", msg);
        lerLinha(linha, MAX_LINHA);
        valido = (sscanf(linha, "%d/%d/%d %d:%d %c",
                         &dia, &mes, &ano, &hora, &min, &lixo) == 5
                  && dataValida(dia, mes, ano)
                  && hora >= 0 && hora <= 23 && min >= 0 && min <= 59);
        if (!valido)
            printf("Data/hora invalida.\n");
    } while (!valido);
    return criarTempo(dia, mes, ano, hora, min, 0);
}

/* Devolve a data/hora atual ou, se o utilizador preferir, uma indicada por ele */
time_t lerMomento(const char *msg)
{
    if (confirmar("Usar a data/hora atual?"))
        return time(NULL);
    return lerDataHora(msg);
}

/* Escreve em buf a data t como texto; t == 0 significa "sem data" */
void formatarData(time_t t, char *buf, int tam, int comHora)
{
    struct tm *info = NULL;

    if (t == 0) {
        snprintf(buf, tam, "-");
        return;
    }
    info = localtime(&t);
    if (comHora)
        strftime(buf, tam, "%d/%m/%Y %H:%M", info);
    else
        strftime(buf, tam, "%d/%m/%Y", info);
}

/* Paginacao: chamada depois de cada linha de uma listagem.
   No ecra, ao fim de cada pagina, pergunta se continua.
   Devolve 0 se o utilizador quiser parar a listagem. */
int continuarPagina(FILE *out, int linhas)
{
    char linha[MAX_LINHA] = "";

    /* num ficheiro nao ha paginas; no ecra so para no fim de cada pagina */
    if (out != stdout || linhas % LINHAS_POR_PAGINA != 0)
        return 1;
    printf("-- Enter para continuar, 's' para sair da listagem: ");
    lerLinha(linha, MAX_LINHA);
    return tolower((unsigned char)linha[0]) != 's';
}

/* Pergunta se a listagem acabada de mostrar deve ser gravada em ficheiro.
   Devolve o ficheiro aberto para escrita, ou NULL se nao for para gravar. */
FILE *perguntarGravarListagem(void)
{
    char nome[MAX_NOME_FICHEIRO] = "";
    FILE *f = NULL;

    printf("\n");
    if (!confirmar("Gravar esta listagem num ficheiro de texto?"))
        return NULL;

    /* pedir o nome ate nao ser vazio */
    do {
        printf("Nome do ficheiro (ex.: listagem.txt): ");
        lerLinha(nome, MAX_NOME_FICHEIRO);
    } while (nome[0] == '\0');

    f = fopen(nome, "w");
    if (f == NULL)
        printf("Erro: nao foi possivel criar o ficheiro '%s'.\n", nome);
    else
        printf("Listagem gravada em '%s'.\n", nome);
    return f;
}
