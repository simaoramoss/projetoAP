/*
 * bicicletas.h - Gestao da frota de bicicletas.
 */
#ifndef BICICLETAS_H
#define BICICLETAS_H

#include "estruturas.h"

void iniciarLoja(Loja *loja);
void libertarLoja(Loja *loja);

const char *nomeTipo(int tipo);
int tipoPorNome(const char *nome);
const char *nomeEstado(char estado);
const char *nomeMotivo(char motivo);
int lerTipo(void);

int contarEstado(const Loja *loja, char estado);
int contarAtivas(const Loja *loja);
Bicicleta *procurarBike(Loja *loja, int id);
Bicicleta *adicionarBike(Loja *loja, int id, int tipo, char estado,
                         time_t registo, time_t terminus);

void registarNovaBike(Loja *loja);
void colocarEmServico(Loja *loja);
void eliminarBike(Loja *loja);
void marcarManutencao(Loja *loja);
void reverterManutencao(Loja *loja);

#endif
