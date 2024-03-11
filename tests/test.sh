#!/bin/bash

# Este script realiza peticiones al servidor para medir los tiempos de respuesta para un número determinado de peticiones.

# Configuración
HOST="0.0.0.0"
PORT=8080
NUM_REQUESTS=1000

# Bucle para hacer peticiones
for (( i=0; i<$NUM_REQUESTS; i++ ))
do
  echo -n "Esta es una petición de prueba" | curl -s -o /dev/null "$HOST:$PORT"
done

echo "Se enviaron $NUM_REQUESTS peticiones a $HOST:$PORT"
