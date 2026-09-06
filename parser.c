#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>




/* linha só com espaços/tabs/quebra */
static int is_blank(const char *s) {
    for (; *s; s++)
        if (!isspace((unsigned char)*s)) return 0;
    return 1;
}





/* token -> int positivo. 0 em sucess; -1 se não numérico ou <= 0. */
static int parse_pos_int(const char *tok, int *out) {
    errno = 0;
    char *end;
    long v = strtol(tok, &end, 10);
    if (end == tok || *end != '\0')               return -1;  /* não numérico / lixo */
    if (errno == ERANGE || v <= 0 || v > INT_MAX)  return -1;  /* fora de faixa / não positivo */
    *out = (int)v;
    return 0;
}






int parse_input(const char *path,
                Task tasks[], int max_tasks, int *n_tasks,
                int *total_time,
                char *errbuf, size_t errlen) {
    FILE *f = fopen(path, "r");
    if (!f) {
        snprintf(errbuf, errlen, "nao foi possivel abrir '%s': %s", path, strerror(errno));
        return -1;
    }

    char line[512];
    int  lineno = 0;

    /* --- Primeira linha não-vazia: tempo total --- */
    int got_total = 0;
    while (fgets(line, sizeof line, f)) {
        lineno++;
        if (is_blank(line)) continue;
        char *tok = strtok(line, " \t\r\n");
        if (!tok || parse_pos_int(tok, total_time) != 0) {
            snprintf(errbuf, errlen, "linha %d: tempo total invalido", lineno);
            fclose(f); return -1;
        }
        if (strtok(NULL, " \t\r\n")) {
            snprintf(errbuf, errlen, "linha %d: campo extra apos o tempo total", lineno);
            fclose(f); return -1;
        }
        got_total = 1;
        break;
    }
    if (!got_total) {
        snprintf(errbuf, errlen, "arquivo vazio: tempo total ausente");
        fclose(f); return -1;
    }

    /*  Demais linhas: uma tarefa por linha  */
    int n = 0;
    while (fgets(line, sizeof line, f)) {
        lineno++;
        if (is_blank(line)) continue;
        if (n >= max_tasks) {
            snprintf(errbuf, errlen, "excedido o maximo de %d tarefas", max_tasks);
            fclose(f); return -1;
        }

        char *name  = strtok(line, " \t\r\n");
        char *sp    = strtok(NULL, " \t\r\n");
        char *sd    = strtok(NULL, " \t\r\n");
        char *sb    = strtok(NULL, " \t\r\n");
        char *extra = strtok(NULL, " \t\r\n");

        if (!name || !sp || !sd || !sb) {
            snprintf(errbuf, errlen, "linha %d: campo faltando", lineno);
            fclose(f); return -1;
        }
        if (extra) {
            snprintf(errbuf, errlen, "linha %d: campo extra", lineno);
            fclose(f); return -1;
        }
        if (strlen(name) >= MAX_NAME_LEN) {
            snprintf(errbuf, errlen, "linha %d: nome muito longo", lineno);
            fclose(f); return -1;
        }

        Task *t = &tasks[n];
        strcpy(t->name, name);   /* seguro: strlen(name) < MAX_NAME_LEN */
        if (parse_pos_int(sp, &t->period)   != 0 ||
            parse_pos_int(sd, &t->deadline) != 0 ||
            parse_pos_int(sb, &t->burst)    != 0) {
            snprintf(errbuf, errlen, "linha %d: valor nao numerico ou nao positivo", lineno);
            fclose(f); return -1;
        }
        if (t->deadline > t->period) {
            snprintf(errbuf, errlen, "linha %d: deadline > periodo (D > P)", lineno);
            fclose(f); return -1;
        }
        if (t->burst > t->deadline) {
            snprintf(errbuf, errlen, "linha %d: burst > deadline (C > D)", lineno);
            fclose(f); return -1;
        }
        t->index = n;
        n++;
    }

    if (n == 0) {
        snprintf(errbuf, errlen, "nenhuma tarefa no arquivo");
        fclose(f); return -1;
    }

    fclose(f);
    *n_tasks = n;
    return 0;
} 