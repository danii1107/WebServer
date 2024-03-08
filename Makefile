CC=gcc
CFLAGS=-Iincludes -Wall -Wextra -Werror -pedantic #-fsanitize=address -g3
DEBUG=gdb
DEPS = includes/http.h includes/methods.h includes/pool.h includes/utils.h includes/picohttpparser.h includes/types.h includes/sockets.h includes/log.h
OBJDIR = obj
LIBDIR = lib
LIBRARY = $(LIBDIR)/picohttpparser.a $(LIBDIR)/p1_2322_03.a
OBJ_SERVER = $(OBJDIR)/http.o $(OBJDIR)/methods.o $(OBJDIR)/pool.o $(OBJDIR)/utils.o $(OBJDIR)/picohttpparser.o $(OBJDIR)/sockets.o $(OBJDIR)/server.o $(OBJDIR)/log.o
ZIP_URL=https://github.com/alber1997/AUXFOLDER/raw/main/root.zip
ZIP_FILE=root.zip
UNZIP_DIR=root

all: verificar_root_o_zip descomprimir_zip server

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

$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

debug:
	$(DEBUG) server

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./server

clean:
	rm -f $(OBJDIR)/*.o $(LIBDIR)/*.a server $(ZIP_FILE)
