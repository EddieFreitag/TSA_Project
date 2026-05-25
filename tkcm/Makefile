CFLAGS += -Wall -Wextra -O3 -std=c11

all: clean paper chlorine experiments

paper:
	$(CC) $(CFLAGS) -o paper paper.c tkcm.c -lm

chlorine:
	$(CC) $(CFLAGS) -o chlorine chlorine.c tkcm.c -lm

experiments:
	$(CC) $(CFLAGS) -o experiments experiments.c tkcm.c data.c -lm

clean:
	rm -rf *.o ./paper ./chlorine ./experiments
