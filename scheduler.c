#include "scheduler.h"

// Estado ativo de cada tarefa durante a simulação do voo. //
typedef struct {
    int active;         /* há instância viva agora? */
    int remaining;      /* rajada restante da instância atual */
    int abs_deadline;   /* deadline absoluto da instância atual */
    int next_arrival;   /* próximo instante de chegada */
} Run;




// Anexa um segmento ao resultado (com proteção de limite). //
static void push_seg(SimResult *r, int task, int duration, SegKind kind) {
    if (duration <= 0) return;
    if (r->n_segments >= MAX_SEGMENTS) return;   // teto de segurança
    Segment *s = &r->segments[r->n_segments++];
    s->task = task; s->duration = duration; s->kind = kind; // assign? Revisar essa linha*
}


static int priority_key(SchedAlgo algo, const Task *t, const Run *r) {
    return (algo == SCHED_RATE) ? t->period : r->abs_deadline;
}

//--------------------------------------------%----------------------------------------------------------------------


/* Escolhe a tarefa pronta de maior prioridade. -1 se ninguém pronto.
   Empate se resolve pela ordem do arquivo: iteramos 0..n-1 com "<" estrito,
   então quem aparece primeiro vence naturalmente. */
static int pick(SchedAlgo algo, const Task *tasks, const Run *run, int n) {int best = -1, best_key = 0; // inicializa com lixo, mas só será usado se best != -1
    for (int i = 0; i < n; i++) {
        if (!run[i].active) continue; // ignora tarefas inativas
        int key = priority_key(algo, &tasks[i], &run[i]);
        if (best == -1 || key < best_key) { best = i; best_key = key; }
    }
    return best;
}




void simulate(SchedAlgo algo, const Task tasks[], int n_tasks,
              int total_time, SimResult *out) {
    out->algo = algo;
    out->tasks = tasks;
    out->n_tasks = n_tasks;
    out->n_segments = 0;

    Run run[MAX_TASKS];
    for (int i = 0; i < n_tasks; i++) {
        run[i].active = 0;
        run[i].remaining = 0;
        run[i].abs_deadline = 0;
        run[i].next_arrival = 0;   
        out->stats[i].lost = out->stats[i].complete = out->stats[i].killed = 0;
    }

    int cur = -1;    /* dono atual da CPU (-1 = idle) */
    int len = 0;     /* duração do segmento aberto */

    for (int t = 0; t < total_time; t++) {
        /* Fase 1: perdas de deadline no instante t. */
        for (int i = 0; i < n_tasks; i++) {
            if (run[i].active && run[i].abs_deadline == t && run[i].remaining > 0) {
                out->stats[i].lost++;
                run[i].active = 0;
                if (i == cur) {                 /* estava rodando: fecha o segmento como L */
                    push_seg(out, cur, len, SEG_LOST);
                    cur = -1; len = 0;
                }                               /* se esperava, some sem segmento */
            }
        }

  




        /* Fase 2: chegadas no instante t. */
        for (int i = 0; i < n_tasks; i++) {
            if (run[i].next_arrival == t) {
                run[i].active = 1;
                run[i].remaining = tasks[i].burst;
                run[i].abs_deadline = t + tasks[i].deadline;
                run[i].next_arrival += tasks[i].period;
            }
        }
      





        /* Fase 3: quem roda em [t, t+1). */
        int win = pick(algo, tasks, run, n_tasks);
        if (win != cur) {
            if (cur != -1)          push_seg(out, cur, len, SEG_HALTED); /* preempção */
            else if (len > 0)       push_seg(out, -1, len, SEG_IDLE);
            cur = win; len = 0;
        }



        /* Fase 4: executa 1 u.t. */
        if (win != -1) {
            run[win].remaining--;
            len++;
            if (run[win].remaining == 0) {      // terminou dentro do prazo */ --> Melhor cenário
                out->stats[win].complete++;
                run[win].active = 0;
                push_seg(out, win, len, SEG_FINISHED);
                cur = -1; len = 0;
            }
        } else {
            len++;   /* idle acumulando */
        }
    }



    /* Fim da simulação: fecha o segmento aberto e conta os mortos. */
    if (cur != -1) {
        out->stats[cur].killed++;
        run[cur].active = 0;
        push_seg(out, cur, len, SEG_KILLED);
    } else if (len > 0) {
        push_seg(out, -1, len, SEG_IDLE);
    }
    for (int i = 0; i < n_tasks; i++)
        if (run[i].active) out->stats[i].killed++;   /* vivos mas parados no fim */
}