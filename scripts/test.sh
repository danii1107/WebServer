#!/bin/bash

cd ../
make

./server &

SERVER_PID=$!

# Array para almacenar los PIDs de los clientes
declare -a CLIENT_PIDS

# Ejecutar el cliente 100 veces con mensajes diferentes
for i in {1..1000}; do
    ./client "Mensaje $i" &
    CLIENT_PIDS+=($!)
done

# Esperar un poco para que todos los clientes puedan conectarse
sleep 0.5

echo "El servidor está soportando $i conexiones simultáneas"

# Matar procesos cliente
for PID in "${CLIENT_PIDS[@]}"; do
    kill $PID
done

sleep 0.25

# Comprobar si todos los ficheros contienen un mensaje distinto
output=$(find . -type f -regextype posix-extended -regex './[0-9]+' -exec md5sum {} + | sort | uniq -d)

# Comprobar si la variable output está vacía
if [ -z "$output" ]; then
    echo "No hay ficheros con contenido duplicado."
else
    echo "$output"
fi

sleep 0.5
# Matar el proceso del servidor al finalizar
kill $SERVER_PID

make clean

