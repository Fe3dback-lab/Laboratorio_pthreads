#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define TOTAL_ITEMS 200000   // cantidad total de elementos producidos en cada ejecucion

// Estado compartido (se reinicia en cada ejecucion)
int* buffer;
int tam_buffer;
int in, out, cuenta;
int productores_activos;
int items_por_productor;
int* vistos;     // veces que se consumio el valor v
int* conteo;     // lementos consumidos por el consumidor i

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t no_lleno = PTHREAD_COND_INITIALIZER;
pthread_cond_t no_vacio = PTHREAD_COND_INITIALIZER;

void* productor(void* arg) {
    int id = *(int*) arg;

    for (int j = 0; j < items_por_productor; j++) {
        int valor = id * items_por_productor + j;   // valores unicos

        pthread_mutex_lock(&mtx);
        while (cuenta == tam_buffer) {
            pthread_cond_wait(&no_lleno, &mtx);
        }
        buffer[in] = valor;
        in = (in + 1) % tam_buffer;
        cuenta++;
        pthread_cond_signal(&no_vacio);
        pthread_mutex_unlock(&mtx);
    }

    pthread_mutex_lock(&mtx);
    productores_activos--;
    pthread_cond_broadcast(&no_vacio);
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*) arg;

    while (1) {
        pthread_mutex_lock(&mtx);
        while (cuenta == 0 && productores_activos > 0) {
            pthread_cond_wait(&no_vacio, &mtx);
        }
        if (cuenta == 0) {
            pthread_mutex_unlock(&mtx);
            break;
        }

        int valor = buffer[out];
        out = (out + 1) % tam_buffer;
        cuenta--;
        vistos[valor]++;
        conteo[id]++;
        pthread_cond_signal(&no_lleno);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

double ejecutar(int P, int C, int tam) {
    int total = (TOTAL_ITEMS / P) * P;

    // Reiniciar el estado compartido
    tam_buffer = tam;
    buffer = malloc(tam * sizeof(int));
    in = 0;
    out = 0;
    cuenta = 0;
    productores_activos = P;
    items_por_productor = total / P;
    vistos = calloc(total, sizeof(int));
    conteo = calloc(C, sizeof(int));

    pthread_t* hilos_p = malloc(P * sizeof(pthread_t));
    pthread_t* hilos_c = malloc(C * sizeof(pthread_t));
    int* ids_p = malloc(P * sizeof(int));
    int* ids_c = malloc(C * sizeof(int));

    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < P; i++) {
        ids_p[i] = i;
        pthread_create(&hilos_p[i], NULL, productor, &ids_p[i]);
    }
    for (int i = 0; i < C; i++) {
        ids_c[i] = i;
        pthread_create(&hilos_c[i], NULL, consumidor, &ids_c[i]);
    }
    for (int i = 0; i < P; i++) pthread_join(hilos_p[i], NULL);
    for (int i = 0; i < C; i++) pthread_join(hilos_c[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) * 1000.0 + (fin.tv_nsec - inicio.tv_nsec) / 1000000.0;

    // Verificacion
    int faltantes = 0, duplicados = 0, consumidos = 0;
    for (int v = 0; v < total; v++) {
        if (vistos[v] == 0) faltantes++;
        if (vistos[v] > 1) duplicados++;
        consumidos += vistos[v];
    }
    int correcto = (faltantes == 0 && duplicados == 0 && consumidos == total);

    printf("Buffer = %4d | Productores = %d | Consumidores = %d | Producidos = %7d | Consumidos = %7d | Tiempo = %10.3f ms | %s\n", tam, P, C, total, total, tiempo, correcto ? "Correcto" : "ERROR");

    free(buffer);
    free(vistos);
    free(conteo);
    free(hilos_p);
    free(hilos_c);
    free(ids_p);
    free(ids_c);
    return tiempo;
}

int main(void) {
    int tamanos_buffer[4] = {1, 8, 64, 1024};
    int configs[5][2] = {{1, 1}, {2, 2}, {4, 4}, {2, 4}, {4, 2}};   //productores, consumidores

    printf("Elementos totales por ejecucion: %d\n\n", TOTAL_ITEMS);

    for (int b = 0; b < 4; b++) {
        for (int c = 0; c < 5; c++) {
            ejecutar(configs[c][0], configs[c][1], tamanos_buffer[b]);
        }
        printf("\n");
    }

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&no_lleno);
    pthread_cond_destroy(&no_vacio);
    return 0;
}
