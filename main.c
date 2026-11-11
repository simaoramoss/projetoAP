/*
 * main.c - BikeRent: gestao de aluguer de bicicletas.
 * Projeto Pratico de Algoritmos e Programacao 2026/27.
 *
 * Menus da aplicacao: balcao (componente operacional), gestao e listagens.
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "estruturas.h"
#include "utils.h"
#include "bicicletas.h"
#include "alugueres.h"
#include "ficheiros.h"
#include "listagens.h"

/* Opcoes do menu principal */
#define OP_SAIR      0
#define OP_BALCAO    1
#define OP_GESTAO    2
#define OP_LISTAGENS 3
#define OP_GRAVAR    4

/* Grava os dados e informa o utilizador do resultado */
static void gravar(const Loja *loja)
{
    if (gravarDados(loja))
        printf("Dados gravados com sucesso.\n");
    else
        printf("Erro: nao foi possivel gravar os dados.\n");
}

/* Pede um intervalo de dias valido (fim nao anterior ao inicio) */
static void lerIntervalo(time_t *ini, time_t *fim)
{
    do {
        *ini = lerDia("Dia de inicio", 0);
        *fim = lerDia("Dia de fim", 1);
        if (*fim < *ini)
            printf("O dia de fim nao pode ser anterior ao dia de inicio.\n");
    } while (*fim < *ini);
}

/* Funcionalidade 18: consulta um aluguer a partir do seu numero */
static void consultarAluguer(Loja *loja)
{
    int numero = lerInteiro("\nNumero do aluguer: ", 1, INT_MAX);
    Aluguer *a = procurarAluguer(loja, numero);
    FILE *f = NULL;

    if (a == NULL) {
        printf("Nao existe nenhum aluguer com o numero %d.\n", numero);
        return;
    }
    imprimirTalao(a, stdout);
    /* funcionalidade 19: gravar o que foi mostrado, se pedido */
    f = perguntarGravarListagem();
    if (f != NULL) {
        imprimirTalao(a, f);
        fclose(f);
    }
}

/* Componente operacional, para utilizacao no balcao da loja */
static void menuBalcao(Loja *loja)
{
    int op = 0;
    FILE *f = NULL;

    /* repetir o menu ate ser escolhida a opcao de voltar */
    do {
        printf("\n----- BALCAO -----\n");
        printf("1 - Levantamento de bicicleta\n");
        printf("2 - Devolucao de bicicleta\n");
        printf("3 - Cancelar aluguer\n");
        printf("4 - Consultar aluguer\n");
        printf("5 - Alugueres em curso\n");
        printf("0 - Voltar\n");
        op = lerInteiro("Opcao: ", 0, 5);

        switch (op) {
        case 1: registarLevantamento(loja); break;
        case 2: registarDevolucao(loja); break;
        case 3: cancelarAluguer(loja); break;
        case 4: consultarAluguer(loja); break;
        case 5:
            listarAlugueresEmCurso(loja, stdout);
            f = perguntarGravarListagem();
            if (f != NULL) {
                listarAlugueresEmCurso(loja, f);
                fclose(f);
            }
            break;
        default: break;
        }
        if (op != 0)
            pausa();
    } while (op != 0);
}

/* Componente de gestao: frota e precos */
static void menuGestao(Loja *loja)
{
    int op = 0;

    /* repetir o menu ate ser escolhida a opcao de voltar */
    do {
        printf("\n----- GESTAO -----\n");
        printf("1 - Registar nova bicicleta\n");
        printf("2 - Colocar bicicleta nova em servico\n");
        printf("3 - Eliminar bicicleta (fim de vida)\n");
        printf("4 - Marcar bicicleta para manutencao\n");
        printf("5 - Terminar/reverter manutencao\n");
        printf("6 - Alterar precos\n");
        printf("0 - Voltar\n");
        op = lerInteiro("Opcao: ", 0, 6);

        switch (op) {
        case 1: registarNovaBike(loja); break;
        case 2: colocarEmServico(loja); break;
        case 3: eliminarBike(loja); break;
        case 4: marcarManutencao(loja); break;
        case 5: reverterManutencao(loja); break;
        case 6: alterarPrecos(loja); break;
        default: break;
        }
        if (op != 0)
            pausa();
    } while (op != 0);
}

/* Listagens e estatisticas; cada uma pode ser gravada em ficheiro de texto.
   Para gravar, a mesma funcao e chamada outra vez com o ficheiro como saida. */
static void menuListagens(Loja *loja)
{
    int op = 0;
    time_t ini = 0, fim = 0;
    FILE *f = NULL;

    /* repetir o menu ate ser escolhida a opcao de voltar */
    do {
        printf("\n----- LISTAGENS E ESTATISTICAS -----\n");
        printf("1 - Todas as bicicletas e respetivo estado\n");
        printf("2 - Alugueres realizados num intervalo de datas\n");
        printf("3 - Alugueres em curso\n");
        printf("4 - Total faturado entre duas datas\n");
        printf("5 - Tipo de bicicleta mais alugado num intervalo\n");
        printf("6 - Duracao media dos alugueres\n");
        printf("7 - Tarifario\n");
        printf("0 - Voltar\n");
        op = lerInteiro("Opcao: ", 0, 7);

        /* as opcoes 2, 4 e 5 precisam de um intervalo de datas */
        if (op == 2 || op == 4 || op == 5)
            lerIntervalo(&ini, &fim);

        switch (op) {
        case 1:
            listarBikes(loja, stdout);
            f = perguntarGravarListagem();
            if (f != NULL) listarBikes(loja, f);
            break;
        case 2:
            listarAlugueresIntervalo(loja, stdout, ini, fim);
            f = perguntarGravarListagem();
            if (f != NULL) listarAlugueresIntervalo(loja, f, ini, fim);
            break;
        case 3:
            listarAlugueresEmCurso(loja, stdout);
            f = perguntarGravarListagem();
            if (f != NULL) listarAlugueresEmCurso(loja, f);
            break;
        case 4:
            mostrarFaturacao(loja, stdout, ini, fim);
            f = perguntarGravarListagem();
            if (f != NULL) mostrarFaturacao(loja, f, ini, fim);
            break;
        case 5:
            mostrarTipoMaisAlugado(loja, stdout, ini, fim);
            f = perguntarGravarListagem();
            if (f != NULL) mostrarTipoMaisAlugado(loja, f, ini, fim);
            break;
        case 6:
            mostrarDuracaoMedia(loja, stdout);
            f = perguntarGravarListagem();
            if (f != NULL) mostrarDuracaoMedia(loja, f);
            break;
        case 7:
            mostrarTarifario(loja, stdout);
            f = perguntarGravarListagem();
            if (f != NULL) mostrarTarifario(loja, f);
            break;
        default: break;
        }
        /* fechar o ficheiro da listagem, se foi criado */
        if (f != NULL) {
            fclose(f);
            f = NULL;
        }
        if (op != 0)
            pausa();
    } while (op != 0);
}

int main(void)
{
    Loja loja;
    int op = 0;
    int ok = 0;

    iniciarLoja(&loja);
    printf("===== BikeRent - Gestao de Aluguer de Bicicletas =====\n");

    /* com dados de uma sessao anterior carrega os binarios;
       caso contrario faz a instalacao a partir dos ficheiros .txt */
    if (existemDadosGravados()) {
        ok = carregarDados(&loja);
        if (!ok)
            printf("Erro: os ficheiros binarios estao em falta ou corrompidos.\n");
    } else {
        ok = instalar(&loja);
        /* gravar logo os binarios para as sessoes seguintes */
        if (ok)
            gravar(&loja);
    }
    if (!ok) {
        libertarLoja(&loja);
        return EXIT_FAILURE;
    }

    /* menu principal: repetir ate ser escolhida a opcao de sair */
    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        /* funcionalidade 4: mostrar sempre o resumo da frota */
        printf("Disponiveis: %d | Alugadas: %d | Em manutencao: %d\n",
               contarEstado(&loja, ESTADO_DISPONIVEL),
               contarEstado(&loja, ESTADO_ALUGADA),
               contarEstado(&loja, ESTADO_MANUTENCAO));
        printf("%d - Balcao (levantamentos e devolucoes)\n", OP_BALCAO);
        printf("%d - Gestao (frota e precos)\n", OP_GESTAO);
        printf("%d - Listagens e estatisticas\n", OP_LISTAGENS);
        printf("%d - Gravar dados\n", OP_GRAVAR);
        printf("%d - Sair\n", OP_SAIR);
        op = lerInteiro("Opcao: ", OP_SAIR, OP_GRAVAR);

        switch (op) {
        case OP_BALCAO:    menuBalcao(&loja); break;
        case OP_GESTAO:    menuGestao(&loja); break;
        case OP_LISTAGENS: menuListagens(&loja); break;
        case OP_GRAVAR:    gravar(&loja); break;
        default: break;
        }
    } while (op != OP_SAIR);

    /* funcionalidade 20: gravar obrigatoriamente ao sair */
    gravar(&loja);
    libertarLoja(&loja);
    return EXIT_SUCCESS;
}
