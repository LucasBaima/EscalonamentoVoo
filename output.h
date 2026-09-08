#ifndef OUTPUT_H
#define OUTPUT_H

#include <stddef.h>        /* size_t */
#include "scheduler.h"     /* SimResult, SchedAlgo (via task.h) */

/* Grava o resultado da simulação em 'path', no formato do enunciado:
   EXECUTION BY <ALGO>, seguido de LOST DEADLINES, COMPLETE EXECUTION e KILLED.

   Retorna 0 em sucesso. Se o arquivo não puder ser aberto para escrita,
   escreve a mensagem em errbuf e retorna != 0 — sem NUnca criar saída pela metade.
   */
int write_output(const char *path, const SimResult *result, char *errbuf, size_t errlen);

#endif /* OUTPUT_H */