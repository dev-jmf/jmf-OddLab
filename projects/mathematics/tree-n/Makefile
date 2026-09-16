CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
LDLIBS = -lX11

OBJECTS = main.o tree.o embedding.o tree_n.o web_server.o

all: tree-n

tree-n: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

clean:
	rm -f $(OBJECTS) tree-n

.PHONY: all clean