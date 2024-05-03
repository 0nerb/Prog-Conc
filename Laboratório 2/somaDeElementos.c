#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define TESTE

//vetor global
float *vetor;

//estrutura das threads
typedef struct 
{
    int id;
    long int tamanhoBloco;
}threadArgumentos;

typedef struct 
{
    float somaVetor;
}somaElementos;



//fluxo das threads
void * tarefa(void * arg){
    threadArgumentos *args = (threadArgumentos *) arg;

    somaElementos *somaElementos;
    somaElementos = malloc(sizeof(somaElementos));
    if(somaElementos==NULL) {
      pthread_exit(NULL);
    }
    int inicio = args ->id * args -> tamanhoBloco; //inicio do bloco
    int fim = inicio + args -> tamanhoBloco; //fim do bloco
    somaElementos ->somaVetor =0;
    for(int i = inicio; i<fim; i++ ){
        
        somaElementos->somaVetor = somaElementos->somaVetor + vetor[i];
         
    }
    
    pthread_exit((void*) somaElementos);
}

int main(int argc, char *argv[]){
    float valorSoma = 0.0;
    int tamanhoVetor, tamanhoBloco, nThreads;

    pthread_t *threadId;
    somaElementos *somaGeral;

    somaGeral = (somaElementos*) malloc(sizeof(somaElementos));
    if(somaGeral==NULL) {
      return 1;
    }
#ifdef TESTE
    float somaCorreta = 0.0;
#endif

    if(argc < 2){
        fprintf(stderr, "digite %s <numero de threads>\n", argv[0]);
        return 2;
    }

    //numero de threads
    nThreads = atoi(argv[1]);
    printf("Numero de threads = %d\n", nThreads);

    //tamanho do vetor
    printf("Digite a dimensão do vetor\n");
    scanf("%d", &tamanhoVetor);
    printf("O tamanho do vetor é: %d\n", tamanhoVetor);

    //aloca o vetor
    vetor = (float*)malloc(sizeof(float)*tamanhoVetor);
    if (vetor == NULL)
    {
        fprintf(stderr, "ERRO--malloc do vetor\n");
        return 3;

    }
    //preenche o vetor de entrada

    for (int i = 0; i < tamanhoVetor; i++)
    {
        printf("Digite os elementos do vetor: \n");
        scanf("%f", &vetor[i]);
    }
    
#ifdef TESTE
    printf("Digite a soma correta: \n");
    scanf("%f", &somaCorreta);
    for(int i = 0; i<tamanhoVetor; i++){
        printf("%f ", vetor[i]);
    }
#endif
    //cria as threads
    threadId = (pthread_t *) malloc(sizeof(pthread_t)*nThreads);
    if (threadId == NULL)
    {
        fprintf(stderr, "ERRO-- malloc das threads\n");
        return 4;
    }
    tamanhoBloco = tamanhoVetor/nThreads;
    if (!tamanhoBloco)
    {
        printf("\nA quantidade de threads é maior que o numero de elementos do vetor\n");
        return 5;
    }

    for(int i = 0; i < nThreads; i++){
        //aloca e preenche argumentos para a thread
        threadArgumentos *args = (threadArgumentos*) malloc(sizeof(threadArgumentos));
        if (args == NULL)
        {
            printf("ERRO-- malloc de preenchimento das threads\n");
            pthread_exit(NULL);

        }
        args ->id = i;
        args ->tamanhoBloco = tamanhoBloco;
        if (pthread_create(threadId+i, NULL, tarefa, (void*)args))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            pthread_exit(NULL);
        }
    }

    //processa a ultima parte do vetor
    if (tamanhoVetor%nThreads){
        puts("\nMain com tarefa\n");
        
        for(int i = tamanhoVetor - (tamanhoBloco%nThreads); i < tamanhoVetor; i++ ){
            valorSoma += vetor[i];
        }
    }
    

    for(int i = 0; i < nThreads; i++){
        pthread_join(*(threadId+i),(void**) &somaGeral);
        valorSoma = somaGeral ->somaVetor + valorSoma;
    }

    
    
    printf("\n\n A soma dos elementos do vetor é: %f\n", valorSoma);

#ifdef TESTE
    printf("\n\n A soma correta dos elementos do vetor é: %f\n", somaCorreta);
#endif

    free(vetor);
    free(threadId);
    
    return 0;

}