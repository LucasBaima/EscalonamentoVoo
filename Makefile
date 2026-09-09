CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2    #aVisos do compilador
TARGET  = scheduler
OBJS    = main.o parser.o scheduler.o output.o

# Alvo padrão (primeiro do arquivo): 'make' sem argumento cai aqui.
all: $(TARGET) 

# Liga os objetos no executável final.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


main.o:      main.c task.h parser.h scheduler.h output.h
	$(CC) $(CFLAGS) -c main.c

parser.o:    parser.c parser.h task.h
	$(CC) $(CFLAGS) -c parser.c

scheduler.o: scheduler.c scheduler.h task.h
	$(CC) $(CFLAGS) -c scheduler.c

output.o:    output.c output.h scheduler.h task.h
	$(CC) $(CFLAGS) -c output.c


clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean