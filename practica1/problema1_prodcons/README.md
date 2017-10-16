# Problema 1: Consumidor / Productor
Resolver mediante semáforos el problema de concurrencia de consumidores y productores; se aportan dos soluciones, con el buffer tipo FIFO, y la otra con el buffer tipo LIFO.

Para compilar los binarios:

1. Sitúate en la carpeta practica1/problema1_prodcons.
2. Crea las carpetas `bin` y `obj` con `$ mkdir bin obj`.
3. Compila `$ make`.
4. Se producen las dos soluciones FIFO y LIFO en `bin`.
5. Para limpiar `.o` `$ make clean`, para los binarios tambien `$ make mrproper`.

Para compilar la memoria:

1. Sitúate en la carpeta practica1/problema1_prodcons.
2. Compila `$ make crearpdf`.
3. La memoria se encuentra en `tex`.
4. Para borrar el pdf `$ make limpiarpdf`.
