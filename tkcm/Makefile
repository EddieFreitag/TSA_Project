CFLAGS += -Wall -Wextra -O3 -std=c11

all: clean paper chlorine

paper:
	$(CC) $(CFLAGS) -o paper paper.c tkcm.c

chlorine:
	$(CC) $(CFLAGS) -o chlorine chlorine.c tkcm.c

clean:
	rm -rf *.o ./paper ./chlorine
