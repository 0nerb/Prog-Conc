#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "timer.h"

/*sequencial 1.000 = 0.000421
  sequencial 1.000.000 = 0.200540
*/

#define NTHREADS 8
#define N 100000000
int indiceN = 0;

pthread_mutex_t mutex;


// verifica se o numero é primo
int ehPrimo(long long int n) {
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
        for (int i=3; i<sqrt(n)+1; i+=2){
            if(n%i==0) return 0;
        }
return 1;   
}
//funcao concorrente
void * conta_primos (void * arg){
    int indiceN_local, total = 0; 
    pthread_mutex_lock(&mutex);
    indiceN_local = indiceN;
    indiceN++;
    pthread_mutex_unlock(&mutex);

    while(indiceN_local < N){
        
        if(ehPrimo(indiceN_local)) {
            total ++;
            //printf("total = %d \n", total);
        }
        pthread_mutex_lock(&mutex);
        indiceN_local = indiceN;
        indiceN++;
        pthread_mutex_unlock(&mutex);

    }

    pthread_exit((void *)(intptr_t)total);
}


int main (){
    pthread_t tid[NTHREADS];
    
    int t, id[NTHREADS];
    double inicio, fim, delta;


    GET_TIME(inicio);
    for(t=0; t<NTHREADS; t++) {
        id[t]=t;
        if (pthread_create(&tid[t], NULL, conta_primos, (void *) &id[t])) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
  //--espera todas as threads terminarem
    int totalPrimos = 0;
    for (t=0; t<NTHREADS; t++) {
        int threadPrimos;
        pthread_join(tid[t], (void**) &threadPrimos);
        totalPrimos += threadPrimos;
        
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo: %lf\n", delta);
    pthread_mutex_destroy(&mutex);
    printf("Número de primos = %d\n", totalPrimos);
    return 0;
}


