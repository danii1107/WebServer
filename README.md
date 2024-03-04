# Practica 1 Redes 2 - 2322

## Hecho por la pareja 03 - Daniel Birsan y Diego Rodrigo Aguilar

**Asegúrese de que 'curl' esta instalado en su sitema.**

1. Compilar y enlazar ejecutando en la raíz `make`
2. A continuación usar *./server* para ejecutar el servidor.
3. Se le mostrará la dirección y el puerto al que puede conectarse localmente, puede usar 'curl' o un navegador para hacer requests al sistema.
4. Puede ejecutar `curl -X OPTIONS "http://localhost:puerto"` para ver verbos soportados por el sistema.
5. Ninguna petición puede ser del tipo `http://localhost:puerto/*..*` por seguridad. 

---

**Si se quiere añadir elementos para probar el servidor hágalo en la carpeta descomprimida ya que el cliente no podrá acceder a ubicaciones fuera de 'root'.**

'root' es una carpeta con los archivos multimedia y tests proporcionada en el enunciado para probar el funcionamiento del servidor.

La carpeta 'root' debería descargarse y descomprimirse automáticamente, en caso de error, puede descargarla manualmente desde aquí: [Descargar zip](https://drive.google.com/file/d/18Jh1N-6f-E4vfCYOp1D7nzYBGLZYnR-h/view?usp=sharing), asegúrese de descomprimir este '.zip' y que la carpeta descomprimida está a la altura del 'server.conf'.


- Implementar post
- Argumentos scripts
- Revisar scripts se tiene que indicar env 
- Al fallar script servidor no responde