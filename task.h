#ifndef TASK_H
#define TASK_H

#define MAX_NAME_LEN 32   /* tamanho máximo do nome de uma tarefa */
#define MAX_TASKS    64   /* teto de tarefas por arquivo */


typedef struct {
    char name[MAX_NAME_LEN];
    int  period;    /* P: de quanto em quanto tempo a tarefa reaparece */
    int  deadline;  /* D: prazo relativo, contado a partir da chegada  */
    int  burst;     /* C: unidades de CPU necessárias para terminar    */
    int  index;     /* posição no arquivo (0,1,2...): usado no desempate */
} Task;


/* Algoritmo escolhido na linha de comando. */
typedef enum {
    SCHED_RATE,   /* rate-monotonic: prioridade fixa, pelo período      */
    SCHED_EDF     /* earliest-deadline-first: pelo deadline absoluto     */
} SchedAlgo;

#endif /* TASK_H */