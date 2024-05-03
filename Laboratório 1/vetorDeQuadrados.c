#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define tamanhoVetor 100
                     
#define tamanhoThread 4


typedef struct{
 long int *vetor;
 long int inicio;
 long int fim;
} dadosVetor;


long int *iniciarVetor (long int tamVetor) {
    long int *Vetor = malloc(tamVetor * sizeof(long int));
       for(long int i=0; i < tamVetor; i++){
           Vetor[i] = i+1;
       }
  
   return Vetor;
}


void *elevarQuadrado (void *arg){
   dadosVetor *args = (dadosVetor *)arg;

   for (long int i = args->inicio; i < args->fim; i++) {
       args->vetor[i] *= args->vetor[i];
       //prlong intf("Vetor[%d] = %d \n", i+1 , args->vetor[i]);
   }
  
   return NULL;
}


long int teste(long int *vetor, long int tamanho) {
 long int *vetorTeste = malloc(tamanho * sizeof(long int));


 for (long int i = 0; i < tamanho; i++) {
   vetorTeste[i] = i+1;
   if (!(vetorTeste[i]*vetorTeste[i] == vetor[i])){
       printf("Resultado incorreto\n");   
   }
  
 }
 return 0;
}




int main (){
   long int *vetor = iniciarVetor(tamanhoVetor);

   pthread_t threads[tamanhoThread];
   dadosVetor dados[tamanhoThread];


   long int tamanhoBloco = tamanhoVetor / tamanhoThread;
   for(long int i = 0; i < tamanhoThread; i++)
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
       for (long int i=tamanhoVetor-(tamanhoVetor%tamanhoThread); i<tamanhoVetor; i++)
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
   for(long int i = 0; i < tamanhoThread; i++){
       pthread_join(threads[i], NULL);
   }
   teste(vetor, tamanhoVetor);
   free(vetor);


   return 0;
  
}

