/*
 * alugueres.h - Levantamentos, devolucoes e cancelamentos.
 */
#ifndef ALUGUERES_H
#define ALUGUERES_H

#include <stdio.h>
#include "estruturas.h"

Aluguer *adicionarAluguer(Loja *loja, int idBike, int tipo, time_t inicio);
Aluguer *procurarAluguer(Loja *loja, int numero);
int horasUtilizacao(time_t inicio, time_t fim);
float calcularValor(const Tarifa *tarifa, int horas);
void imprimirTalao(const Aluguer *a, FILE *out);

void registarLevantamento(Loja *loja);
void registarDevolucao(Loja *loja);
void cancelarAluguer(Loja *loja);

#endif
