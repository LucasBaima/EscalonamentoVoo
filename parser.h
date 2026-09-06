#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "task.h"

// Lê e valida o arquivo de entrada//
   
int parse_input(const char *path,Task tasks[], int max_tasks, int *n_tasks,int *total_time, char *errbuf, size_t errlen);

#endif /* PARSER_H */