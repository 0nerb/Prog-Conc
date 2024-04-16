#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

int numThreads; 
float *matriz1, *matriz2, *matrizResultado;
int linhasMatriz1, colunasMatriz1, linhasMatriz2, colunasMatriz2; //dimensoes das matrizes

// fluxo das threads
void *multiplicaConcorrente (void *arg) {
   long int id = (long int) arg;
   float aux = 0; 

   if(colunasMatriz1 == linhasMatriz2) {
      for(int i = id; i < linhasMatriz1; i += numThreads){
         for(int j = 0; j < colunasMatriz2; j++){
            matrizResultado[i*linhasMatriz1+j] = 0;
            for(int z = 0; z < linhasMatriz2; z++){
               aux += matriz1[i*linhasMatriz1+z] * matriz2[z*linhasMatriz2+j];
            }

            matrizResultado[i*linhasMatriz1+j] = aux;
            aux = 0;
         }
      }
   } else {
      fprintf(stderr, "Não é possível multiplicar as matrizes dadas\n");
      pthread_exit(NULL);
   }

   pthread_exit(NULL);
}

// fluxo sequencial 
int main(int argc, char*argv[]) {
   long long int tamMatriz1, tamMatriz2, tamMatrizResultado; //qtde de elementos nas matrizes
   FILE * arquivoEntrada1, * arquivoEntrada2, * arquivoSaida; //descritores dos arquivos
   size_t ret1, ret2, retSaida; //retorno da funcao de leitura nos arquivos 
   double inicio, fim, delta;

   pthread_t *tid; // identificadores das threads

   // --- INICIALIZAÇÃO --- //

   GET_TIME(inicio);

   //recebe os argumentos de entrada
   if(argc < 5) {
      fprintf(stderr, "Digite: %s <arquivo entrada1> <arquivo entrada2> <arquivo saida> <numero threads>\n", argv[0]);
      return 1;
   }

   // recebe o número de threads
   numThreads = atoi(argv[4]);

   //abre o arquivo para leitura binaria da entrada 1 
   arquivoEntrada1 = fopen(argv[1], "rb");
   arquivoEntrada2 = fopen(argv[2], "rb");
   if(!arquivoEntrada1 || !arquivoEntrada2) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

   //le as dimensoes das matrizes
   ret1 = fread(&linhasMatriz1, sizeof(int), 1, arquivoEntrada1);
   if(!ret1) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo 1\n");
      return 3;
   }
   ret1 = fread(&colunasMatriz1, sizeof(int), 1, arquivoEntrada1);
   if(!ret1) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo 1\n");
      return 3;
   }
   ret2 = fread(&linhasMatriz2, sizeof(int), 1, arquivoEntrada2);
   if(!ret2) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo 2\n");
      return 3;
   }
   ret2 = fread(&colunasMatriz2, sizeof(int), 1, arquivoEntrada2);
   if(!ret2) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo 2\n");
      return 3;
   }
   tamMatriz1 = linhasMatriz1 * colunasMatriz1; //calcula a qtde de elementos da matriz de entrada 1
   tamMatriz2 = linhasMatriz2 * colunasMatriz2; //calcula a qtde de elementos da matriz de entrada 2
   tamMatrizResultado = linhasMatriz1 * colunasMatriz2; //calcula a qtde de elementos da matriz de saida


   //aloca memoria para as matrizes
   matriz1 = (float*) malloc(sizeof(float) * tamMatriz1);
   matriz2 = (float*) malloc(sizeof(float) * tamMatriz2);
   matrizResultado = (float*) malloc(sizeof(float) * tamMatrizResultado);
   if(!matriz1 || !matriz2 || !matrizResultado) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret1 = fread(matriz1, sizeof(float), tamMatriz1, arquivoEntrada1);
    if(ret1 < tamMatriz1) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz 1\n");
      return 4;
   }
   ret2 = fread(matriz2, sizeof(float), tamMatriz2, arquivoEntrada2);
   if(ret2 < tamMatriz2) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz 2\n");
      return 4;
   }

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicialização: %lf\n", delta);

   // --- PROCESSAMENTO --- //

   GET_TIME(inicio);

   tid = (pthread_t *) malloc(sizeof(pthread_t) * numThreads);
   if(tid == NULL){
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }

   // cria threads
   for(long int i = 0; i < numThreads; i++){
      if(pthread_create(tid+i, NULL, multiplicaConcorrente, (void*) i)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 3;
      }
   }

   // aguardar o término das threads
   for(long int i = 0; i < numThreads; i++){
      if(pthread_join(*(tid+i), NULL)){
         fprintf(stderr, "ERRO--pthread_join\n");
      }
   }

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo processamento: %lf\n", delta);

   // imprime matriz resultante
   // for(int i=0; i<linhasMatriz1; i++) { 
   //    for(int j=0; j<colunasMatriz2; j++)
   //      fprintf(stdout, "%.6f ", matrizResultado[i*linhasMatriz1+j]);
   //    fprintf(stdout, "\n");
   // }

   // --- FINALIZAÇÃO --- //

   GET_TIME(inicio);

   //abre o arquivo para escrita binaria
   arquivoSaida = fopen(argv[3], "wb");
   if(!arquivoSaida) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve numero de linhas e de colunas
   retSaida = fwrite(&linhasMatriz1, sizeof(int), 1, arquivoSaida);
   retSaida = fwrite(&colunasMatriz2, sizeof(int), 1, arquivoSaida);

   // escreve os elementos da matriz
   retSaida = fwrite(matrizResultado, sizeof(float), tamMatrizResultado, arquivoSaida);
   if(retSaida < tamMatrizResultado) {
      fprintf(stderr, "Erro de escrita no arquivo\n");
      return 4;
   }

   //finaliza o uso das variaveis
   fclose(arquivoEntrada1);
   fclose(arquivoEntrada2);
   fclose(arquivoSaida);
   free(tid);
   free(matriz1);
   free(matriz2);
   free(matrizResultado);

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo finalização: %lf\n", delta);

   return 0;
}
