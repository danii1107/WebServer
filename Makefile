CC=gcc
CFLAGS=-Iincludes -Wall -pedantic
DEBUG=gdb
DEPS = includes/*.h 
OBJDIR = obj
OBJ_SERVER = $(OBJDIR)/server.o $(OBJDIR)/sockets.o $(OBJDIR)/pool.o $(OBJDIR)/picohttpparser.o $(OBJDIR)/http.o

all: server

$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

debugs:
	$(DEBUG) server

clean:
	rm -f $(OBJDIR)/*.o server
