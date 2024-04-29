#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define tamanhoVetor 10
#define tamanhoThread 4


typedef struct{
 int *vetor;
 int inicio;
 int fim;
} dadosVetor;


int *iniciarVetor (int tamVetor) {
    int *Vetor = malloc(tamVetor * sizeof(int));
       for(int i=0; i < tamVetor; i++){
           Vetor[i] = i+1;
       }
  
   return Vetor;
}


void *elevarQuadrado (void *arg){
   dadosVetor *args = (dadosVetor *)arg;


   for (int i = args->inicio; i < args->fim; i++) {
       args->vetor[i] *= args->vetor[i];
       printf("Vetor[%d] = %d \n", i+1 , args->vetor[i]);
   }
  
   return NULL;
}


int teste(int *vetor, int tamanho) {
 int *vetorTeste = malloc(tamanho * sizeof(int));


 for (int i = 0; i < tamanho; i++) {
   vetorTeste[i] = i+1;
   if (!(vetorTeste[i]*vetorTeste[i] == vetor[i])){
       printf("Resultado incorreto\n");   
   }
  
 }
 return 0;
}




int main (){
   int *vetor = iniciarVetor(tamanhoVetor);


   


   pthread_t threads[tamanhoThread];
   dadosVetor dados[tamanhoThread];


   int tamanhoBloco = tamanhoVetor / tamanhoThread;
   for(int i = 0; i < tamanhoThread; i++)
   {
       dados[i].vetor = vetor;
       dados[i].inicio = (i) * tamanhoBloco;
       dados[i].fim = (i+1) * tamanhoBloco;


       
       if(pthread_create(&threads[i], NULL, elevarQuadrado, &dados[i]))
       {
           printf("ERRO: PTHREAD_CREATE()\n");
       }
   }
    if (tamanhoBloco%tamanhoThread)
   {
       for (int i=tamanhoVetor-(tamanhoVetor%tamanhoThread); i<tamanhoVetor; i++)
       {
           dados[i].vetor = vetor;
           dados[i].inicio = (i);
           dados[i].fim = (i+1);
           if(pthread_create(&threads[0], NULL, elevarQuadrado, &dados[i]))
           {
           printf("ERRO: PTHREAD_CREATE()\n");
           }
           
       }
      
   }
   for(int i = 0; i < tamanhoThread; i++){
       pthread_join(threads[i], NULL);
   }
   teste(vetor, tamanhoVetor);
   free(vetor);


   return 0;
  
}

