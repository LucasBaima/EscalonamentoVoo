#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "task.h"
#include "parser.h"
#include "scheduler.h"
#include "output.h"

#ifndef LOGIN
#define LOGIN "jlogb"   
#endif

// Ponto único de saída por erro: se caímos aqui, nenhum .out foi criado. */
static int fail(const char *msg) {
    fprintf(stderr, "erro: %s\n", msg);
    return 1;
}


static int resolve_algo(const char *arg, SchedAlgo *algo) {
    static const struct { const char *name; SchedAlgo value; } table[] = { //struct anônima
        { "rate", SCHED_RATE },
        { "edf",  SCHED_EDF  },   // <Nunca muda ! talvez definir como constante global? 
    };
    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++) { // busca linear? 
        if (strcmp(arg, table[i].name) == 0) {
            *algo = table[i].value;
            return 0;
        }
    }
    return -1;
}




int main(int argc, char *argv[]) {
    if (argc != 3)
        return fail("uso: ./scheduler <rate|edf> <arquivo>");

    SchedAlgo algo;
    if (resolve_algo(argv[1], &algo) != 0)
        return fail("algoritmo invalido: use 'rate' ou 'edf'");

    Task tasks[MAX_TASKS];
    int  n_tasks, total_time;
    char errbuf[256];

    if (parse_input(argv[2], tasks, MAX_TASKS, &n_tasks, &total_time,errbuf, sizeof errbuf) != 0)
        return fail(errbuf);

    /* Nome de saída derivado do argumento já validado: rate_<login>.out / edf_<login>.out */
    char out_path[64];
    snprintf(out_path, sizeof out_path, "%s_%s.out", argv[1], LOGIN);

    SimResult result;
    simulate(algo, tasks, n_tasks, total_time, &result);

    if (write_output(out_path, &result, errbuf, sizeof errbuf) != 0)
        return fail(errbuf);

    return 0;
}