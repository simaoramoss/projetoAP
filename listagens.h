/*
 * listagens.h - Listagens, tarifario e estatisticas.
 * Todas as funcoes escrevem em "out": stdout mostra no ecra,
 * um ficheiro aberto grava a listagem em texto.
 */
#ifndef LISTAGENS_H
#define LISTAGENS_H

#include <stdio.h>
#include "estruturas.h"

void listarBikes(const Loja *loja, FILE *out);
void listarAlugueresIntervalo(const Loja *loja, FILE *out, time_t ini, time_t fim);
void listarAlugueresEmCurso(const Loja *loja, FILE *out);
void mostrarTarifario(const Loja *loja, FILE *out);
void mostrarFaturacao(const Loja *loja, FILE *out, time_t ini, time_t fim);
void mostrarTipoMaisAlugado(const Loja *loja, FILE *out, time_t ini, time_t fim);
void mostrarDuracaoMedia(const Loja *loja, FILE *out);
void alterarPrecos(Loja *loja);

#endif
