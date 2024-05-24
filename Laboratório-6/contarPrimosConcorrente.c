#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <math.h>


sem_t vazio, cheio; //controle de buffer
sem_t mutex; // semaforo unico para exclusão mútua
long long int *buffer; //armazenar os números
int tam_buffer; // tamanho do buffer

typedef struct {
    FILE *arquivo;
} ThreadArgs;

int is_prime(long long int numero) {
      if (numero <= 1) {
    return 0;
  }

  for (int i = 2; i <= sqrt(numero); i++) {
    if (numero % i == 0) {
      return 0;
    }
  }

  return 1;
}

//Inserir elemento no buffer
void inserirBuffer(long long int numero) {
    static int PosInsercao = 0; 
    sem_wait(&vazio); // Aguarda o slot vazio no buffer 
    sem_wait(&mutex);
    buffer[PosInsercao] = numero; // insere o número no buffer
    PosInsercao = (PosInsercao + 1) % tam_buffer; // Atualiza a posição de inserção
    sem_post(&mutex);
    sem_post(&cheio); //sinaliza um slot cheio
}

//Remover elemento do buffer
long long int removerBuffer() {
    static int PosRemocao = 0; // Posição de remoção
    long long int number;
    sem_wait(&cheio); // espera o slot cheio no buffer
    sem_wait(&mutex); 
    if (buffer[PosRemocao] == -1) { // Verifica se é o fim do arquivo
        sem_post(&mutex);
        sem_post(&vazio); 
	    sem_post(&cheio); 
        return -1;
    }
    number = buffer[PosRemocao]; // Pega o número do buffer
    buffer[PosRemocao] = 0; // Apaga o número do buffer (opcional)
    PosRemocao = (PosRemocao + 1) % tam_buffer; // Atualiza a posição de remoção 
    sem_post(&mutex);
    sem_post(&vazio); 
    return number;
}

void *produtor(void *arg){
    ThreadArgs *args = (ThreadArgs *) arg;
    long long int aux;
    while (fread(&aux, sizeof(long long int), 1, args->arquivo)) {
        inserirBuffer(aux);
    }
    inserirBuffer(-1); // Insere um número inválido para indicar o fim do arquivo

    if(fclose(args->arquivo)) {
        fprintf(stderr, "Erro ao fechar o arquivo\n");
        exit(1);
    }
    free(args);
    
    pthread_exit(NULL);
}

void *consumidor(void *arg) { 
    int *primos = (int *) malloc(sizeof(int));
    if (!primos) {
        fprintf(stderr, "Erro de alocação\n");
        pthread_exit(NULL);
    } 
    *primos = 0;

    long long int numero;
    while (1) {
        numero = removerBuffer();
	
        if (numero == -1){ 
            break;
        } // Verifica se é o fim do arquivo
        if (is_prime(numero)){ 
            (*primos)++;
        }    
    }
   

    pthread_exit((void*) primos);
}


int main (int argc, char *argv[]) {
    // Variáveis locais
    int primosMax = 0 , thread_primosMax = 0, n_consumidores = 0, primosTotais = 0;
    pthread_t *tid;
    int *return_primos = NULL;

    // Recebe e valida os parâmetros de entrada
    if (argc < 4) {
        fprintf(stderr, "Digite: %s num_thread_consumidor tam_buffer arquivo_entrada\n", argv[0]);
        return 1;
    }
    
    n_consumidores = atoi(argv[1]);
    tam_buffer = atoi(argv[2]);
    //valida o tamanho dos argumentos
    if (n_consumidores < 1 || tam_buffer < 1) {
        fprintf(stderr, " O número de thread consumidor e do tamanho do buffer deve ser maior que 0\n");
        return 2;
    }
    // abre o arquivo de entrada
    FILE *arquivo = fopen(argv[3], "rb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada\n");
        return 3;
    }
    // inicializa o buffer
    buffer = (long long int *) malloc(sizeof(long long int) * tam_buffer);
    if (!buffer) {
        fprintf(stderr, "Erro de alocação do Buffer\n");
        return 4;
    }
    
    // Aloca espaço para as threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * n_consumidores+1); // n_consumidores + 1 produtor
    if (!tid) {
        fprintf(stderr, "Erro de alocação\n");
        return 5;
    }

    // Inicializa as variáveis de controle
    sem_init(&vazio, 0, tam_buffer); // Buffer vazio e com tamanho recebido 
    sem_init(&cheio, 0, 0); // Buffer cheio inicialmente com 0 elementos 
    sem_init(&mutex, 0, 1); // Inicializa o semáforo


    // Cria a thread produtora
    ThreadArgs *arg = (ThreadArgs *) malloc(sizeof(ThreadArgs));
    arg->arquivo = arquivo;
    if (pthread_create(tid, NULL, produtor, (void *) arg)) {
        fprintf(stderr, "Erro na criação da thread produtora\n");
        return 6;
    }
    // cria as threads consumidoras
    for (int i = 1; i <= n_consumidores; i++) {
        if (pthread_create(tid+i, NULL, consumidor, NULL)) {
            fprintf(stderr, "Erro na criação da thread consumidora %d\n", i);
            return 7;
        }
    }

    // Aguarda o término das threads consumidoras e produtora
    for (int i = 1; i <= n_consumidores; i++) {
        if (pthread_join(*(tid+i), (void**) &return_primos)) {
            fprintf(stderr, "Erro no join das threads\n");
            return 8;
        }
        if (return_primos) {
            primosTotais += *return_primos;
            printf("Thread %d achou - %d primos\n", i, *return_primos); //quantos primos cada thread achou
            free(return_primos);
        }
    }

    // Exibe o resultado
    printf("Número totais de Primos na sequência: %d\n", primosTotais);
    

    // Libera a memória alocada
    free(tid);
    free(buffer);

    // Finaliza os semáforos
    sem_destroy(&vazio);
    sem_destroy(&cheio);
    sem_destroy(&mutex);

    return 0;
}