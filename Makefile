CC=gcc
CFLAGS=-Iincludes -Wall -Werror -pedantic
DEBUG=gdb
DEPS = includes/types.h includes/sockets.h includes/pool.h
OBJDIR = obj
OBJ_CLIENT = $(OBJDIR)/client.o $(OBJDIR)/sockets.o
OBJ_SERVER = $(OBJDIR)/server.o $(OBJDIR)/sockets.o $(OBJDIR)/pool.o

all: client server

$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

debugs:
	$(DEBUG) server

debugc:
	$(DEBUG) client

clean:
	rm -f $(OBJDIR)/*.o client server
