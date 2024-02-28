CC=gcc
CFLAGS=-Iincludes
DEPS = includes/types.h includes/sockets.h
OBJDIR = obj
OBJ_CLIENT = $(OBJDIR)/client.o
OBJ_SERVER = $(OBJDIR)/server.o $(OBJDIR)/sockets.o

all: client server

$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

exes:
	./server

exec:
	./client

clean:
	rm -f $(OBJDIR)/*.o client server
	find . -type f -regex '.*/[0-9].*' -delete
