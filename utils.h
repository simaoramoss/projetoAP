/*
 * utils.h - Leitura validada de dados, datas e apoio as listagens.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <time.h>

void lerLinha(char *buf, int tam);
int lerInteiro(const char *msg, int min, int max);
float lerFloat(const char *msg, float min, float max);
char lerOpcaoChar(const char *msg, const char *validos);
int confirmar(const char *msg);
void pausa(void);

time_t lerDia(const char *msg, int fimDoDia);
time_t lerDataHora(const char *msg);
time_t lerMomento(const char *msg);
void formatarData(time_t t, char *buf, int tam, int comHora);

int continuarPagina(FILE *out, int linhas);
FILE *perguntarGravarListagem(void);

#endif
