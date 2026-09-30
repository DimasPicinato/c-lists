CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = app.exe
SRC = main.c lista.c listaDupla.c fila.c pilha.c
HDR = lista.h listaDupla.h fila.h pilha.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	.\$(TARGET)

clean:
	del /Q $(TARGET)

.PHONY: all run clean
