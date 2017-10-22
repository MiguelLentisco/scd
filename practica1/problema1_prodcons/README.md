# Problema 1: Consumidor / Productor
Resolver mediante semáforos el problema de concurrencia de consumidores y productores; se aportan dos soluciones, con el buffer tipo FIFO, y la otra con el buffer tipo LIFO.

Para compilar los binarios:

1. Crea las carpetas `bin` y `obj` con `$ mkdir bin obj`.
2. Compila `$ make`.
3. Se producen las dos soluciones FIFO y LIFO en `bin`.
4. Para limpiar `.o` `$ make clean`, para los binarios tambien `$ make mrproper`.

Para compilar la memoria:

1. Compila `$ make crearpdf`.
2. La memoria se encuentra en `tex`.
3. Para borrar el pdf `$ make limpiarpdf`.

# Extra: modificaciones

Se incluyen modificaciones para poder ejecutar el programa con cuantos consumidores o productores queramos.
