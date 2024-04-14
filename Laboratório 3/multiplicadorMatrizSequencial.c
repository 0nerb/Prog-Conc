#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIPO_DADO float

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

void multiplicar_matrizes_sequencial(TIPO_DADO *matriz_a, int n_linhas_a, int n_colunas_a,
                                      TIPO_DADO *matriz_b, int n_linhas_b, int n_colunas_b,
                                      TIPO_DADO *matriz_c) {
  for (int i = 0; i < n_linhas_a; i++) {
    for (int j = 0; j < n_colunas_b; j++) {
      matriz_c[i * n_colunas_b + j] = 0;
      for (int k = 0; k < n_colunas_a; k++) {
        matriz_c[i * n_colunas_b + j] += matriz_a[i * n_colunas_a + k] * matriz_b[k * n_colunas_b + j];
      }
    }
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
  if (argc != 4) {
    printf("Uso: %s <arquivo_matriz_a> <arquivo_matriz_b> <arquivo_matriz_c>\n", argv[0]);
    exit(1);
  }

  TIPO_DADO *matriz_a, *matriz_b, *matriz_c;
  int n_linhas_a, n_colunas_a, n_linhas_b, n_colunas_b;

  // Le as matrizes dos arquivos
  carregar_matriz(argv[1], &matriz_a, &n_linhas_a, &n_colunas_a);
  carregar_matriz(argv[2], &matriz_b, &n_linhas_b, &n_colunas_b);

  // Verifica se as matrizes são compatíveis para multiplicação
  if (n_colunas_a != n_linhas_b) {
    printf("Erro: As dimensões das matrizes não são compatíveis\n");
    exit(1);
  }

  // Aloca memória para a matriz resultante
  matriz_c = (TIPO_DADO *)malloc(n_linhas_a * n_colunas_b * sizeof(TIPO_DADO));
  if (matriz_c == NULL) {
    printf("Erro ao alocar memória para a matriz resultante\n");
    exit(1);
  }

  // Início da medição do tempo
  clock_t tempo_inicial = clock();

  // Multiplicação sequencial das matrizes
  multiplicar_matrizes_sequencial(matriz_a, n_linhas_a, n_colunas_a, matriz_b, n_linhas_b, n_colunas_b, matriz_c);

  // Fim da medição do tempo
  clock_t tempo_final = clock();
  double tempo_execucao = (double)(tempo_final - tempo_inicial) / CLOCKS_PER_SEC;

  // Imprime as matrizes
  //imprimir_matriz("A", matriz_a, n_linhas_a, n_colunas_a);
  //imprimir_matriz("B", matriz_b, n_linhas_b, n_colunas_b);
  //imprimir_matriz("C", matriz_c, n_linhas_a, n_colunas_b);

  // Imprime o tempo de execução
  printf("Tempo de execução: %f segundos\n", tempo_execucao);

  // Salva a matriz resultante no arquivo
  salvar_matriz(argv[3], matriz_c, n_linhas_a, n_colunas_b);

  // Libera a memória alocada
  free(matriz_a);
  free(matriz_b);
  free(matriz_c);

  return 0;
}
