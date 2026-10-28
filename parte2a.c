#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
 
typedef struct {
    const int* A;
    int inicio;
    int fin;
    int* hist;
    pthread_mutex_t* mtx;
} datos_hist;
 
void* histograma_thread(void* arg) {
    datos_hist* datos = (datos_hist*) arg;
    for (int i = datos->inicio; i < datos->fin; i++) {
        int categoria = datos->A[i] / 10;
        pthread_mutex_lock(datos->mtx);
        datos->hist[categoria]++;
        pthread_mutex_unlock(datos->mtx);
    }
    return NULL;
}
 
// Secuencial
void histograma_secuencial(const int A[], int n, int hist[]) {
    for (int c = 0; c < 10; c++) hist[c] = 0;
    for (int i = 0; i < n; i++) {
        hist[A[i] / 10]++;
    }
}
 
// Paralelo 
void histograma_paralelo(const int A[], int n, int hist[], int num_threads) {
    for (int c = 0; c < 10; c++) hist[c] = 0;
 
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, NULL);
 
    pthread_t* hilos = malloc(num_threads * sizeof(pthread_t));
    datos_hist* datos = malloc(num_threads * sizeof(datos_hist));
 
    int base = n / num_threads;
    int resto = n % num_threads;
    int inicio = 0;
 
    for (int i = 0; i < num_threads; i++) {
        int cantidad = base;
        if (i < resto) cantidad++;
 
        datos[i].A = A;
        datos[i].inicio = inicio;
        datos[i].fin = inicio + cantidad;
        datos[i].hist = hist;
        datos[i].mtx = &mtx;
 
        pthread_create(&hilos[i], NULL, histograma_thread, &datos[i]);
        inicio += cantidad;
    }
 
    for (int i = 0; i < num_threads; i++) {
        pthread_join(hilos[i], NULL);
    }
 
    pthread_mutex_destroy(&mtx);
    free(hilos);
    int c=0;
    for(int i=0; i<10; i++){
        printf("Categoria %d: %d\n", i, hist[i]);
        c+= hist[i];
    }
    if(c == n){
        printf("Suma de categorias = %d (n = %d) -> Correcto\n", c, n);
    } else {
        printf("Suma de categorias = %d (n = %d) -> ERROR\n", c, n);
    }
    free(datos);
}
 
int main() {
    int tamanos[5] = {107, 1003, 10007, 100019, 1000003};
    int hilos_prueba[5] = {1, 2, 4, 8, 16};
 
    srand(42);
    for (int k = 0; k < 5; k++) {
        int n = tamanos[k];
 
        int* A = malloc(n * sizeof(int));
        if (A == NULL) {
            printf("Error al reservar memoria.\n");
            return 1;
        }
        for (int i = 0; i < n; i++) {
            A[i] = rand() % 100;
        }
 
        int hist[10];
        struct timespec inicio, fin;
 
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        histograma_secuencial(A, n, hist);
        clock_gettime(CLOCK_MONOTONIC, &fin);
 
        double tiempo = (fin.tv_sec - inicio.tv_sec) * 1000.0 + (fin.tv_nsec - inicio.tv_nsec) / 1000000.0;
 
        printf("\nTamaño n = %d\n", n);
        printf("Tiempo secuencial = %.6f ms\n", tiempo);
 
        for (int p = 0; p < 5; p++) {
            int num_threads = hilos_prueba[p];
 
            clock_gettime(CLOCK_MONOTONIC, &inicio);
            histograma_paralelo(A, n, hist, num_threads);
            clock_gettime(CLOCK_MONOTONIC, &fin);
 
            double tiempo_par = (fin.tv_sec - inicio.tv_sec) * 1000.0 + (fin.tv_nsec - inicio.tv_nsec) / 1000000.0;
 
            printf("Hilos = %d | Tiempo = %.6f ms\n", num_threads, tiempo_par);
        }
 
        free(A);
    }
 
    return 0;
}
