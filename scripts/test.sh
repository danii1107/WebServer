#!/bin/bash

cd ../
make

./server &

SERVER_PID=$!

# Array para almacenar los PIDs de los clientes
declare -a CLIENT_PIDS

# Ejecutar el cliente 100 veces con mensajes diferentes
for i in {1..100}; do
    ./client &
    CLIENT_PIDS+=($!)
done

# Esperar un poco para que todos los clientes puedan conectarse
sleep 0.5

sleep 3

echo "El servidor está soportando $i conexiones simultáneas"

# Matar procesos cliente
for PID in "${CLIENT_PIDS[@]}"; do
    kill -s SIGINT $PID
done

sleep 0.25

sleep 0.5
# Matar el proceso del servidor al finalizar
kill -s SIGINT $SERVER_PID

make clean

