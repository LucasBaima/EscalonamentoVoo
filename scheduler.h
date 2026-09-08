#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

#define MAX_SEGMENTS 4096   // teto de trechos na linha do tempo //

/* Por que um segmento de CPU terminou. */
typedef enum {
    SEG_FINISHED,   /* F */
    SEG_HALTED,     /* H */
    SEG_LOST,       /* L */
    SEG_KILLED,     /* K */
    SEG_IDLE        /* CPU ociosa (sem letra) */
} SegKind;


/* Um trecho contínuo da linha do tempo. */
typedef struct {
    int     task;       /* índice da tarefa; -1 se idle */
    int     duration;   /* quantas u.t. durou */
    SegKind kind;
} Segment;

// Contadores das tarefas (cada uma) */
typedef struct {
    int lost;       /* LOST DEADLINES  */
    int complete;   /* COMPLETE EXECUTION */
    int killed;     /* KILLED */
} TaskStat;



/* Resultado completo da simulação — tudo que o output precisa. */
typedef struct {
    SchedAlgo   algo;
    const Task *tasks;
    int         n_tasks;

    Segment segments[MAX_SEGMENTS];
    int     n_segments;

    TaskStat stats[MAX_TASKS];
} SimResult;




/* Roda a simulação. Não faz I/O; para entrada válida não falha (void). */
void simulate(SchedAlgo algo, const Task tasks[], int n_tasks, int total_time, SimResult *out);

#endif /* SCHEDULER_H */