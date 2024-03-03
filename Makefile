CC=gcc
CFLAGS=-Iincludes -Wall -Werror -Wextra -pedantic -fsanitize=address -g3
DEBUG=gdb
DEPS = includes/http.h includes/methods.h includes/pool.h includes/utils.h includes/picohttpparser.h includes/types.h includes/sockets.h  
OBJDIR = obj
OBJ_SERVER = $(OBJDIR)/http.o $(OBJDIR)/methods.o $(OBJDIR)/pool.o $(OBJDIR)/utils.o $(OBJDIR)/picohttpparser.o $(OBJDIR)/sockets.o $(OBJDIR)/server.o 

all: server

$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

debugs:
	$(DEBUG) server

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./server

clean:
	rm -f $(OBJDIR)/*.o server
