#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*Implemente um programa com 5 threads:
A thread 1 imprime a frase “Oi Maria!”;
A thread 2 imprime a frase “Oi José!”;
A thread 3 imprime a frase “Sente-se por favor.”;
A thread 4 imprime a frase “Até mais José!”;
A thread 5 imprime a frase “Até mais Maria!”.*/

#define NTHREADS 5

//variaveis globais
int x = 0;
pthread_mutex_t x_mutex;
//pthread_cond_t x_cond;

void *helloMaria (void *t){
    //printf("hello começou...\n");

    pthread_mutex_lock(&x_mutex);
    printf("HELLO MARIA\n");
    pthread_mutex_unlock(&x_mutex);

    //printf("hello terminou...\n");
    pthread_exit(NULL);
}
void *helloJose (void *t){
    //printf("hello começou...\n");

    pthread_mutex_lock(&x_mutex);
    printf("HELLO JOSE\n");
    pthread_mutex_unlock(&x_mutex);

    //printf("hello terminou...\n");
    pthread_exit(NULL);
}
void *sentar (void *t){
    //printf("sentar começou...\n");

    pthread_mutex_lock(&x_mutex);
    printf("SENTE-SE\n");
    pthread_mutex_unlock(&x_mutex);

    //printf("sentar terminou...\n");
    pthread_exit(NULL);

}

void *byeMaria (void *t){
    //printf("bye começou...\n");

    pthread_mutex_lock(&x_mutex);
    printf("BYE MARIA\n");
    pthread_mutex_unlock(&x_mutex);

    //printf("bye terminou...\n");
    pthread_exit(NULL);

}
void *byeJose (void *t){
    //printf("bye começou...\n");

    pthread_mutex_lock(&x_mutex);
    printf("BYE JOSE\n");
    pthread_mutex_unlock(&x_mutex);

    //printf("bye terminou...\n");
    pthread_exit(NULL);

}

int main (int argc, char *argv[]){
    pthread_t threads[NTHREADS];

    
    pthread_create(&threads[0], NULL, helloJose, NULL);
    pthread_create(&threads[1], NULL, helloMaria, NULL);
    pthread_create(&threads[2], NULL, sentar, NULL);
    pthread_create(&threads[3], NULL, byeJose, NULL);
    pthread_create(&threads[4], NULL, byeMaria, NULL);
    

    for(int i = 0; i < NTHREADS; i++){
        pthread_join(threads[i], NULL);
    }
    printf("\nFIM\n");

    pthread_mutex_destroy(&x_mutex);

    return 0;

}