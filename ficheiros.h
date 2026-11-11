/*
 * ficheiros.h - Instalacao (ficheiros de texto) e persistencia (binarios).
 */
#ifndef FICHEIROS_H
#define FICHEIROS_H

#include "estruturas.h"

int existemDadosGravados(void);
int instalar(Loja *loja);
int carregarDados(Loja *loja);
int gravarDados(const Loja *loja);

#endif
