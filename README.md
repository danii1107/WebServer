# Practica 1 Redes 2 - 2322

## Hecho por la pareja 03 - Daniel Birsan y Diego Rodrigo Aguilar

**Asegúrese de que 'curl' esta instalado en su sitema.**

1. Compilar y enlazar ejecutando en la raíz `make`.
2. A continuación usar `./server` para ejecutar el servidor.
3. Se le mostrará la dirección y el puerto al que puede conectarse localmente, puede usar 'curl' o un navegador para hacer requests al sistema.
4. Puede ejecutar `curl -X OPTIONS "http://localhost:puerto"` para ver verbos soportados por el sistema.
5. Ninguna petición puede ser del tipo `http://localhost:puerto/*..*` por seguridad.
6. Se crea un fichero *server.log* donde se registra todo lo sucedido internamente en el servidor.
7. `make clean` borrará el ejecutable y root.zip (además de objetos y librerías estáticas) pero no borrará la carpeta descomprimida de pruebas ni el *sever.log*, puede borrar ambos archivos con `rm -rf root` y `rm -rf sever.log` respectivamente.
8. El servidor soporta la ejecución de scripts en Python y PHP, sin embargo, los intérpretes deben estar instalados en su sistema.
9. En el index del servidor verá dos scripts creados por nosotros, donde puede introducir los datos, para el GET un nombre, y para el POST, una temperatura Celsius, se ejecutan presionando los botones o con `curl "http://localhost:puerto/scripts/hola.py?nombre=Oscar" -i` y `curl "http://localhost:puerto/scripts/temperatura.py" -d "temp=30" -i` respectivamente.
10. Puede modificar el campo *max_clients* de *server.conf* si su equipo no soporta los hilos preestablecidos (solo ocurre si se estresa el servidor con un gran número de peticiones simultáneas).
11. Tenga en cuenta que si envía un ctrl+C al servidor mientras está procesando una petición, como la descarga de un vídeo, este no se cerrará hasta que la descarga haya finalizado para evitar fugas de memoria, no mate el proceso durante la ejecución de una petición.

---

**Si se quiere añadir elementos para probar el servidor hágalo en la carpeta descomprimida ya que el cliente no podrá acceder a ubicaciones fuera de 'root'.**

'root' es una carpeta con los archivos multimedia y tests proporcionada en el enunciado para probar el funcionamiento del servidor.

La carpeta 'root' debería descargarse y descomprimirse automáticamente, en caso de error, puede descargarla manualmente desde aquí: [Descargar zip](https://drive.google.com/file/d/1iJrPAn8yQjVflkNlOrujgI9DpgBLNriV/view?usp=sharing), asegúrese de descomprimir este '.zip' y que la carpeta descomprimida está a la altura del 'server.conf'.
