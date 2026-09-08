#include "output.h"
#include <stdio.h>


/* Devolve o nome do algoritmo em texto */
static const char *nome_algoritmo(SchedAlgo algo) {
    if (algo == SCHED_RATE)
        return "RATE";
    else
        return "EDF";
}

/* Devolve a letra do segmento */
static char letra_segmento(SegKind tipo) {
    if (tipo == SEG_FINISHED) return 'F';
    if (tipo == SEG_HALTED)   return 'H';
    if (tipo == SEG_LOST)     return 'L';
    if (tipo == SEG_KILLED)   return 'K';
    return '?';
}

/* Escreve um bloco de contadores (LOST, COMPLETE ou KILLED) */
static void escrever_contadores(FILE *f,
                                const char *titulo,
                                const SimResult *r,
                                size_t offset) {
    fprintf(f, "%s\n", titulo);

    for (int i = 0; i < r->n_tasks; i++) {
        /* Pega o valor do campo usando o deslocamento (offset) */
        int valor = *(const int *)((const char *)&r->stats[i] + offset);

        fprintf(f, "[%s] %d\n", r->tasks[i].name, valor);
    }
}

/* Função principal: grava o resultado no arquivo */
int write_output(const char *path,
                 const SimResult *result,
                 char *errbuf,
                 size_t errlen) {

    FILE *f = fopen(path, "w");
    if (!f) {
        snprintf(errbuf, errlen, "nao foi possivel gravar '%s'", path);
        return -1;
    }

    /* 1. Linha do tempo */
    fprintf(f, "EXECUTION BY %s\n", nome_algoritmo(result->algo));

    for (int i = 0; i < result->n_segments; i++) {
        const Segment *s = &result->segments[i];

        if (s->kind == SEG_IDLE) {
            fprintf(f, "idle for %d units\n", s->duration);
        } else {
            fprintf(f, "[%s] for %d units - %c\n",
                    result->tasks[s->task].name,
                    s->duration,
                    letra_segmento(s->kind));
        }
    }

    /* 2. Contadores */
    escrever_contadores(f, "LOST DEADLINES",    result, offsetof(TaskStat, lost));
    escrever_contadores(f, "COMPLETE EXECUTION", result, offsetof(TaskStat, complete));
    escrever_contadores(f, "KILLED",            result, offsetof(TaskStat, killed));

    fclose(f);
    return 0;
}