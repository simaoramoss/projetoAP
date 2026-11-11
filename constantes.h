/*
 * constantes.h - Constantes da aplicacao BikeRent.
 * Nenhum numero "magico" deve aparecer no resto do codigo.
 */
#ifndef CONSTANTES_H
#define CONSTANTES_H

/* ---------- Personalizacao por grupo ---------- */

/* NN = (soma dos numeros de aluno do grupo) % 100.
   Grupo de 1 aluno, n. 25197: NN = 25197 % 100 = 97. */
#define NN 97

/* Numero maximo de bicicletas (nao terminadas) que o negocio pode gerir */
#define CAPACIDADE_MAX (30 + (NN % 5) * 10)

/* Numero de horas a partir do qual se cobra ao dia */
#define LIMIAR_DIARIO (6 + (NN % 3))

/* Desconto (%) aplicado ao preco a hora nos alugueres longos */
#define DESCONTO_LONGA_DURACAO (5 + (NN % 4) * 5)

/* ---------- Regra de precos ---------- */

#define HORAS_MIN_DESCONTO 4   /* a partir destas horas ha desconto        */
#define HORAS_MINIMAS      1   /* qualquer aluguer paga pelo menos 1 hora  */
#define HORAS_POR_DIA      24
#define SEGUNDOS_POR_HORA  3600
#define SEGUNDOS_POR_MINUTO 60
#define PERCENTAGEM        100.0f

/* ---------- Tipos de bicicleta ---------- */

#define NUM_TIPOS     4
#define TIPO_BTT      0
#define TIPO_ESTRADA  1
#define TIPO_ELETRICA 2
#define TIPO_INFANTIL 3
#define TIPO_INVALIDO (-1)
#define MAX_NOME_TIPO 20

/* ---------- Estados de uma bicicleta ---------- */

#define ESTADO_NOVA       'N'
#define ESTADO_DISPONIVEL 'D'
#define ESTADO_ALUGADA    'A'
#define ESTADO_MANUTENCAO 'M'
#define ESTADO_TERMINADA  'T'
#define ESTADOS_VALIDOS   "NDAMT"

/* ---------- Motivos de manutencao ---------- */

#define MOTIVO_AVARIA  'a'
#define MOTIVO_REVISAO 'r'
#define MOTIVO_OUTRO   'o'
#define MOTIVO_NENHUM  '-'
#define MOTIVOS_VALIDOS "aro"

/* ---------- Estados de um aluguer ---------- */

#define ALUGUER_EM_CURSO  'C'
#define ALUGUER_TERMINADO 'T'
#define ALUGUER_CANCELADO 'X'

/* ---------- Ficheiros ---------- */

#define FICH_BIKES_TXT     "Bicicletas_Instalacao.txt"
#define FICH_TARIFARIO_TXT "Tarifario_instalacao.txt"
#define FICH_BIKES_BIN     "bicicletas.dat"
#define FICH_ALUGUERES_BIN "alugueres.dat"
#define FICH_TARIFARIO_BIN "tarifario.dat"

/* ---------- Diversos ---------- */

#define MAX_LINHA          200  /* tamanho maximo de uma linha lida          */
#define MAX_NOME_FICHEIRO  100
#define TAM_DATA           20   /* "dd/mm/aaaa hh:mm" + terminador           */
#define LINHAS_POR_PAGINA  15   /* paginacao das listagens                   */
#define BLOCO_MEMORIA      20   /* nº de elementos acrescentados no realloc  */
#define VIDA_UTIL_ANOS     5    /* vida prevista das bikes da frota inicial  */
#define ANO_MINIMO         2000
#define ANO_MAXIMO         2100
#define ANO_BASE_TM        1900 /* struct tm conta os anos a partir de 1900  */
#define PRECO_MAXIMO       1000.0f

#endif
