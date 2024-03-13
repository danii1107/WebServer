CC=gcc
DEBUG=gdb
CFLAGS=-Iincludes -Wall -Wextra -Werror -pedantic #-fsanitize=address -g3
DEPS = includes/http.h includes/methods.h includes/pool.h includes/utils.h includes/picohttpparser.h includes/types.h includes/sockets.h includes/log.h
OBJDIR = obj
LIBDIR = lib
SRCLIBDIR = srclib
LIBPICO = $(LIBDIR)/libpicohttpparser.a
LIBHTTP = $(LIBDIR)/libhttp.a
LIBPOOL = $(LIBDIR)/libpool.a
LIBSOCK = $(LIBDIR)/libsockets.a
LIBLOG = $(LIBDIR)/liblog.a 
OBJ_PICOHTTPPARSER = $(OBJDIR)/picohttpparser.o
OBJ_HTTP = $(OBJDIR)/http.o $(OBJDIR)/methods.o $(OBJDIR)/utils.o $(OBJDIR)/log.o $(OBJDIR)/picohttpparser.o
OBJ_POOL = $(OBJDIR)/pool.o $(OBJDIR)/log.o $(OBJDIR)/http.o
OBJ_SOCK = $(OBJDIR)/sockets.o $(OBJDIR)/log.o
OBJ_LOG = $(OBJDIR)/log.o
OBJ_SERVER = $(OBJDIR)/server.o
ZIP_URL=https://github.com/alber1997/AUXFOLDER/raw/main/rooot.zip
ZIP_FILE=rooot.zip
UNZIP_DIR=root

all: verificar_root_o_zip descomprimir_zip libpicohttpparser libpool libsockets libhttp liblog server

verificar_root_o_zip:
	@if [ -d $(UNZIP_DIR) ]; then \
		echo "La carpeta $(UNZIP_DIR) ya existe. Omitiendo descarga y descompresión."; \
	elif [ -f $(ZIP_FILE) ]; then \
        echo "El archivo ZIP ya existe. Omitiendo descarga."; \
    else \
        echo "El archivo ZIP no existe. Descargando..."; \
        curl -L $(ZIP_URL) -o $(ZIP_FILE); \
    fi

descomprimir_zip:
	@if [ ! -d $(UNZIP_DIR) ]; then \
        echo "Descomprimiendo el archivo ZIP..."; \
        unzip $(ZIP_FILE) -d $(UNZIP_DIR); \
        echo "Descompresión completada."; \
        rm -rf $(ZIP_FILE); \
    else \
        echo "La carpeta $(UNZIP_DIR) ya existe. Omitiendo descompresión."; \
    fi

$(shell mkdir -p $(OBJDIR) $(LIBDIR))

$(OBJDIR)/%.o: $(SRCLIBDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libpicohttpparser: $(OBJ_PICOHTTPPARSER)
	ar rcs $(LIBPICO) $^

libhttp: $(OBJ_HTTP)
	ar rcs $(LIBHTTP) $^

libpool: $(OBJ_POOL)
	ar rcs $(LIBPOOL) $^

libsockets: $(OBJ_SOCK)
	ar rcs $(LIBSOCK) $^

liblog: $(OBJ_LOG)
	ar rcs $(LIBLOG) $^

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS) -L$(LIBDIR) -lpicohttpparser -lpthread -lpool -lhttp -lsockets -llog

debug:
	$(DEBUG) server

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./server

clean:
	rm -f server $(OBJDIR)/*.o $(LIBDIR)/*.a $(ZIP_FILE)
