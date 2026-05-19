CFLAGS += -Wall -Wextra -O3 -std=c11

all: clean paper chlorine

paper:
	$(CC) $(CFLAGS) -o paper paper.c tkcm.c -lm

chlorine:
	$(CC) $(CFLAGS) -o chlorine chlorine.c tkcm.c -lm

clean:
	rm -rf *.o ./paper ./chlorine
