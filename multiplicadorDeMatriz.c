#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define TIPO_DADO float

typedef struct {
  TIPO_DADO *matriz_a;
  int n_linhas_a;
  int n_colunas_a;
  TIPO_DADO *matriz_b;
  int n_linhas_b;
  int n_colunas_b;
  TIPO_DADO *matriz_c;
  int inicio_bloco;
  int fim_bloco;
  int *contagem_threads;
  int *flag_terminado;
} ParametrosThread;

// Funcao que le a matriz de um arquivo
void carregar_matriz(char *nome_arquivo, TIPO_DADO **matriz, int *n_linhas, int *n_colunas) {
  
  FILE *arquivo = fopen(nome_arquivo, "rb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
    exit(1);
  }

  fread(n_linhas, sizeof(int), 1, arquivo);
  fread(n_colunas, sizeof(int), 1, arquivo);
  

  *matriz = (TIPO_DADO *)malloc((*n_linhas) * (*n_colunas) * sizeof(TIPO_DADO));
  if (*matriz == NULL) {
    printf("Erro ao alocar memória para a matriz\n");
    exit(1);
  }

  fread(*matriz, sizeof(TIPO_DADO), (*n_linhas) * (*n_colunas), arquivo);
   
  fclose(arquivo);
}

// Funcao que escreve a matriz no arquivo
void salvar_matriz(char *nome_arquivo, TIPO_DADO *matriz, int n_linhas, int n_colunas) {
  FILE *arquivo = fopen(nome_arquivo, "wb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
    exit(1);
  }

  fwrite(&n_linhas, sizeof(int), 1, arquivo);
  fwrite(&n_colunas, sizeof(int), 1, arquivo);
  fwrite(matriz, sizeof(TIPO_DADO), n_linhas * n_colunas, arquivo);
  fclose(arquivo);
}

// Funcao que multiplica as matrizes
void *multiplicar_bloco(void *arg) {
  ParametrosThread *parametros = (ParametrosThread *)arg;

  int i, j, k;
  for (i = parametros->inicio_bloco; i < parametros->fim_bloco; i++) {
    for (j = 0; j < parametros->n_colunas_b; j++) {
      parametros->matriz_c[i * parametros->n_colunas_b + j] = 0;
      for (k = 0; k < parametros->n_colunas_a; k++) {
        parametros->matriz_c[i * parametros->n_colunas_b + j] +=
            parametros->matriz_a[i * parametros->n_colunas_a + k] *
            parametros->matriz_b[k * parametros->n_colunas_b + j];
      }
    }
  }

  // Incrementa o contador de threads
  __sync_fetch_and_add(parametros->contagem_threads, 1);

  // Espera as threads acabarem
  while (!*parametros->flag_terminado) {
  }

  return NULL;
}

// Funcao concorrente
void multiplicar_matrizes_concorrente(TIPO_DADO *matriz_a, int n_linhas_a, int n_colunas_a,
                                      TIPO_DADO *matriz_b, int n_linhas_b, int n_colunas_b,
                                      TIPO_DADO *matriz_c, int n_threads) {
  pthread_t threads[n_threads];
  ParametrosThread parametros[n_threads];
  int contagem_threads = 0;
  int flag_terminado = 0;

  int tamanho_bloco = n_linhas_a / n_threads;
  for (int i = 0; i < n_threads; i++) {
    parametros[i].matriz_a = matriz_a;
    parametros[i].n_linhas_a = n_linhas_a;
    parametros[i].n_colunas_a = n_colunas_a;
    parametros[i].matriz_b = matriz_b;
    parametros[i].n_linhas_b = n_linhas_b;
    parametros[i].n_colunas_b = n_colunas_b;
    parametros[i].matriz_c = matriz_c;
    parametros[i].inicio_bloco = i * tamanho_bloco;
    parametros[i].fim_bloco = (i + 1) * tamanho_bloco;
    if (i == n_threads - 1) {
      parametros[i].fim_bloco = n_linhas_a;
    }
    parametros[i].contagem_threads = &contagem_threads;
    parametros[i].flag_terminado = &flag_terminado;

    pthread_create(&threads[i], NULL, multiplicar_bloco, &parametros[i]);
  }

  // Espera todas as threads acabarem
  while (contagem_threads < n_threads) {}

  // Set flag to indicate all threads are finished
  flag_terminado = 1;

 
  for (int i = 0; i < n_threads; i++) {
    pthread_join(threads[i], NULL);
  }


  
}

void imprimir_matriz(char *nome_matriz, TIPO_DADO *matriz, int n_linhas, int n_colunas) {
  printf("Matriz %s:\n", nome_matriz);
  for (int i = 0; i < n_linhas; i++) {
    for (int j = 0; j < n_colunas; j++) {
      printf("%f ", matriz[i * n_colunas + j]);
    }
    printf("\n");
  }
}


int main(int argc, char *argv[]) {
  if (argc != 5) {
    printf("Uso: %s <arquivo_matriz_a> <arquivo_matriz_b> <arquivo_matriz_c> <numero_threads>\n", argv[0]);
    exit(1);
  }

  int n_threads = atoi(argv[4]);

  TIPO_DADO *matriz_a, *matriz_b, *matriz_c;
  int n_linhas_a, n_colunas_a, n_linhas_b, n_colunas_b;

  // Read matrices from files
  carregar_matriz(argv[1], &matriz_a, &n_linhas_a, &n_colunas_a);
  carregar_matriz(argv[2], &matriz_b, &n_linhas_b, &n_colunas_b);

  // Check if matrices are compatible for multiplication
  if (n_colunas_a != n_linhas_b) {
    printf("Erro: As dimensões das matrizes não são compatíveis\n");
    exit(1);
  }

  // Allocate memory for the result matrix
  matriz_c = (TIPO_DADO *)malloc(n_linhas_a * n_colunas_b * sizeof(TIPO_DADO));
  if (matriz_c == NULL) {
    printf("Erro ao alocar memória para a matriz resultante\n");
    exit(1);
  }

  // Start clock to measure execution time
  clock_t tempo_inicial = clock();

  // Multiply matrices concurrently
  multiplicar_matrizes_concorrente(matriz_a, n_linhas_a, n_colunas_a, matriz_b, n_linhas_b, n_colunas_b, matriz_c, n_threads);

  // Stop clock and calculate execution time
  clock_t tempo_final = clock();
  double tempo_execucao = (double)(tempo_final - tempo_inicial) / CLOCKS_PER_SEC;

  imprimir_matriz("A", matriz_a, n_linhas_a, n_colunas_a);
  imprimir_matriz("B", matriz_b, n_linhas_b, n_colunas_b);
  imprimir_matriz("C", matriz_c, n_linhas_a, n_colunas_b);


  // Print execution time
  printf("Tempo de execução: %f segundos\n", tempo_execucao);

  // Save the result matrix to a file
  salvar_matriz(argv[3], matriz_c, n_linhas_a, n_colunas_b);

  // Free allocated memory
  free(matriz_a);
  free(matriz_b);
  free(matriz_c);

  return 0;
}
