/*
 * estruturas.h - Estruturas de dados da aplicacao BikeRent.
 */
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <time.h>
#include "constantes.h"

/* Uma bicicleta da frota */
typedef struct {
    int id;               /* numero da bicicleta (> 0, unico)                   */
    int tipo;             /* TIPO_BTT, TIPO_ESTRADA, TIPO_ELETRICA, TIPO_INFANTIL */
    char estado;          /* 'N' nova, 'D' disponivel, 'A' alugada,
                             'M' manutencao, 'T' terminada                      */
    char motivo;          /* so em manutencao: 'a' avaria, 'r' revisao,
                             'o' outro; caso contrario '-'                      */
    time_t dataRegisto;   /* data de registo no sistema                         */
    time_t dataTerminus;  /* data prevista de fim de operacao/vida              */
    time_t dataEstado;    /* data em que entrou no estado atual
                             (para 'T' e a data de eliminacao)                  */
} Bicicleta;

/* Um aluguer (em curso, terminado ou cancelado) */
typedef struct {
    int numero;           /* numero do aluguer (> 0, sequencial)                */
    int idBike;           /* id da bicicleta alugada                            */
    int tipo;             /* tipo da bicicleta (copiado para as estatisticas)   */
    char estado;          /* 'C' em curso, 'T' terminado, 'X' cancelado         */
    time_t inicio;        /* data/hora do levantamento                          */
    time_t fim;           /* data/hora da devolucao (0 enquanto em curso)       */
    float valor;          /* valor pago em euros (0 enquanto em curso)          */
} Aluguer;

/* Preco de um tipo de bicicleta */
typedef struct {
    float precoHora;      /* euros por hora (>= 0) */
    float precoDia;       /* euros por dia  (>= 0) */
} Tarifa;

/* Todos os dados da loja; e passada por parametro (sem variaveis globais) */
typedef struct {
    Bicicleta *bikes;       /* vetor dinamico com todas as bicicletas          */
    int numBikes;           /* numero de bicicletas no vetor                   */
    int capBikes;           /* numero de posicoes reservadas no vetor          */
    int proximoIdBike;      /* id a atribuir a proxima bicicleta registada     */

    Aluguer *alugueres;     /* vetor dinamico com todos os alugueres           */
    int numAlugueres;       /* numero de alugueres no vetor                    */
    int capAlugueres;       /* numero de posicoes reservadas no vetor          */
    int proximoNumAluguer;  /* numero a atribuir ao proximo aluguer            */

    Tarifa tarifas[NUM_TIPOS]; /* tarifario, indexado pelo tipo de bicicleta   */
} Loja;

#endif
