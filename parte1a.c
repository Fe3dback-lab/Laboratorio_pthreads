#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>
 
typedef struct {
    long long suma;
    int min;
    int max;
    double prom;
} datos;
 
typedef struct {
    int id;
    int inicio;
    int fin;
    int* A;
    datos resultado;
} datos_thread;
 
// Secuencial
datos operaciones_secuencial(const int A[], int n) {
    datos r;
    r.suma = 0;
    r.min = A[0];
    r.max = A[0];
 
    for (int i = 0; i < n; i++) {
        r.suma += A[i];
        if (A[i] > r.max) r.max = A[i];
        if (A[i] < r.min) r.min = A[i];
    }
 
    r.prom = (double)r.suma / n;
    return r;
}
 
// Funcion que ejecuta cada thread
void* calcular(void* arg) {
    datos_thread* t = (datos_thread*) arg;
 
    long long suma = 0;
    int min = INT_MAX;
    int max = INT_MIN;
 
    for (int i = t->inicio; i < t->fin; i++) {
        suma += t->A[i];
        if (t->A[i] > max) max = t->A[i];
        if (t->A[i] < min) min = t->A[i];
    }
 
    t->resultado.suma = suma;
    t->resultado.min  = min;
    t->resultado.max  = max;
    return NULL;
}
 
// Paralelo
datos operaciones_paralelo(int A[], int n, int num_threads) {
    pthread_t hilos[num_threads];
    datos_thread ids[num_threads];
 
    int base = n / num_threads;
    int resto = n % num_threads;
    int inicio = 0;
 
    for (int i = 0; i < num_threads; i++) {
        int cantidad = base;
        if (i < resto) {
            cantidad++;
        }
 
        ids[i].id = i;
        ids[i].inicio = inicio;
        ids[i].fin = inicio + cantidad;
        ids[i].A = A;
        inicio = ids[i].fin;
 
        pthread_create(&hilos[i], NULL, calcular, &ids[i]);
    }
 
    for (int i = 0; i < num_threads; i++) {
        pthread_join(hilos[i], NULL);
    }
 
    // Combinar resultados parciales
    datos total;
    total.suma = 0;
    total.min = INT_MAX;
    total.max = INT_MIN;
 
    for (int i = 0; i < num_threads; i++) {
        total.suma += ids[i].resultado.suma;
 
        if (ids[i].inicio < ids[i].fin) {
            if (ids[i].resultado.min < total.min) total.min = ids[i].resultado.min;
            if (ids[i].resultado.max > total.max) total.max = ids[i].resultado.max;
        }
    }
 
    total.prom = (double)total.suma / n;
    return total;
}
 
int main() {
    int tamaños[8] = {19, 107, 1003, 10007, 100019, 1000003, 10000019, 100000007};
    int hilos_prueba[5] = {1, 2, 4, 8, 16};
 
    srand(42);
    for (int k = 0; k < 8; k++) {
        int n = tamaños[k];
 
        int *A = malloc((size_t)n * sizeof(int));
 
        for (int i = 0; i < n; i++) {
            A[i] = rand() % 1000;
        }
 
        struct timespec inicio, fin;
        clock_gettime(CLOCK_MONOTONIC, &inicio);
 
        datos s = operaciones_secuencial(A, n);
 
        clock_gettime(CLOCK_MONOTONIC, &fin);
 
        double tiempo = (fin.tv_sec - inicio.tv_sec) * 1000.0 + (fin.tv_nsec - inicio.tv_nsec) / 1000000.0;
 
        printf("\nTamaño n = %d\n", n);
        printf("Suma = %lld\n", s.suma);
        printf("Min = %d\n", s.min);
        printf("Max = %d\n", s.max);
        printf("Promedio = %.2f\n", s.prom);
        printf("Tiempo de ejecucion secuencial = %.6f ms\n", tiempo);
        printf("\n");
 
        //paralela con 1, 2, 4, 8 y 16 threads
        for (int p = 0; p < 5; p++) {
            int num_threads = hilos_prueba[p];
 
            clock_gettime(CLOCK_MONOTONIC, &inicio);
 
            datos par = operaciones_paralelo(A, n, num_threads);
 
            clock_gettime(CLOCK_MONOTONIC, &fin);
 
            double tiempo_par = (fin.tv_sec - inicio.tv_sec) * 1000.0 + (fin.tv_nsec - inicio.tv_nsec) / 1000000.0;
 
            double speedup = tiempo / tiempo_par;
            double eficiencia = speedup / num_threads;
 
            printf("Hilos = %d | Tiempo = %.6f ms | Speedup = %.3f | Eficiencia = %.3f\n", num_threads, tiempo_par, speedup, eficiencia);
            
            if (par.suma != s.suma || par.min != s.min || par.max != s.max) {
                printf("ERROR: el resultado paralelo no coincide con el secuencial\n");
            }

        }
 
        free(A);
    }
 
    return 0;
}
