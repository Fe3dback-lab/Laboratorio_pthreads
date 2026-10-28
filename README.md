# Laboratorio Pthreads: Computación Paralela y Distribuida

Este laboratorio implementa tres programas en C utilizando **Pthreads** para trabajar con programación paralela y concurrencia.

## 1. Archivos del laboratorio

| Archivo | Descripción |
|---|---|
| `parte1a.c` | Calcula la suma, el mínimo, el máximo y el promedio de un arreglo. Compara la versión secuencial con la paralela utilizando diferentes cantidades de hilos. |
| `parte2a.c` | Calcula un histograma de 10 categorías para un arreglo de números. Utiliza hilos y un mutex para proteger el acceso a los contadores compartidos. |
| `parte3.c` | Implementa el problema productor-consumidor mediante un buffer circular, utilizando mutex y variables de condición para sincronizar los hilos. |

## 2. Requisitos

- Linux o un sistema compatible con Pthreads.
- Compilador GCC.

En Ubuntu o Debian, instala las herramientas necesarias con:

```bash
sudo apt install build-essential
```

## 3. Compilación y ejecución

Todos los programas se compilan utilizando la opción `-pthread`.

### Parte I: Operaciones con arreglos

Calcula la suma, el mínimo, el máximo y el promedio de arreglos de diferentes tamaños. Compara los tiempos de ejecución secuencial y paralela, y calcula el speedup y la eficiencia para diferentes cantidades de hilos.

**Compilar y ejecutar:**

```bash
gcc -pthread parte1a.c -o parte1
./parte1
```

### Parte II: Histograma

Calcula un histograma de 10 categorías, desde 0–9 hasta 90–99. La versión paralela utiliza mutex para evitar conflictos entre los hilos al actualizar los contadores. También verifica que la suma de las categorías coincida con la cantidad de elementos procesados.

**Compilar y ejecutar:**

```bash
gcc -pthread parte2a.c -o parte2
./parte2
```

### Parte III: Productor-consumidor

Implementa un sistema donde los productores generan elementos y los consumidores los procesan mediante un buffer circular compartido. Utiliza mutex y variables de condición para coordinar el acceso al buffer y evitar conflictos entre los hilos.

Prueba diferentes tamaños de buffer y combinaciones de productores y consumidores. Al finalizar, verifica que todos los elementos producidos hayan sido consumidos exactamente una vez.

**Compilar y ejecutar:**

```bash
gcc -pthread parte3.c -o parte3
./parte3
```

## 4. Consideraciones

- Los programas muestran resultados y tiempos de ejecución según las pruebas implementadas.
- En la Parte I se comparan las versiones secuencial y paralela.
- En la Parte II se verifica que el histograma procese correctamente todos los elementos.
- En la Parte III se comprueba que los productores y consumidores trabajen correctamente mediante la sincronización de los hilos.
- Los tiempos pueden variar según las características del equipo y la carga del sistema.
