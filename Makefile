CC = gcc

CFLAGS = -Wall -Wextra -std=c11


SRC = encriptado.c


OUT = programa


all: $(OUT)


$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)


clean:
	rm -f $(OUT)
	rm -f *.o
	rm -f Enc-* Des-* resumen.txt

run: $(OUT)
	./$(OUT)

