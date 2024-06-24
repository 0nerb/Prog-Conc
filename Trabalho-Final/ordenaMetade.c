#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"

// Função para mesclar dois subvetores
void merge(int *vetor, int inicio, int meio, int fim) {
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Vetores temporários
    int *esquerda = (int *)malloc(n1 * sizeof(int));
    int *direita = (int *)malloc(n2 * sizeof(int));

    if (esquerda == NULL || direita == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(1);
    }

    // Copia os dados para os vetores temporários
    for (int i = 0; i < n1; i++)
        esquerda[i] = vetor[inicio + i];
    for (int j = 0; j < n2; j++)
        direita[j] = vetor[meio + 1 + j];

    // Mescla os vetores temporários de volta no vetor original
    int i = 0, j = 0, k = inicio;
    while (i < n1 && j < n2) {
        if (esquerda[i] <= direita[j]) {
            vetor[k] = esquerda[i];
            i++;
        } else {
            vetor[k] = direita[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de esquerda[], se houver
    while (i < n1) {
        vetor[k] = esquerda[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de direita[], se houver
    while (j < n2) {
        vetor[k] = direita[j];
        j++;
        k++;
    }

    free(esquerda);
    free(direita);
}

// Função Merge Sort
void merge_sort(int *vetor, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;

        // Ordena a primeira e a segunda metade
        merge_sort(vetor, inicio, meio);
        merge_sort(vetor, meio + 1, fim);

        // Mescla as duas metades ordenadas
        merge(vetor, inicio, meio, fim);
    }
}

int main(int argc, char *argv[]) {

    double inicio, fim, delta; // gerenciadores de tempo

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo de entrada> <arquivo de saída>\n", argv[0]);
        return 1;
    }

    char *nomeArquivoEntrada = argv[1];
    char *nomeArquivoSaida = argv[2];

    // Abre o arquivo para leitura binária
    FILE *arquivoEntrada = fopen(nomeArquivoEntrada, "rb");
    if (arquivoEntrada == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada\n");
        return 2;
    }

    // Lê o tamanho do vetor do arquivo
    long long int tamanho;
    if (fread(&tamanho, sizeof(long long int), 1, arquivoEntrada) != 1) {
        fprintf(stderr, "Erro ao ler o tamanho do vetor do arquivo de entrada\n");
        fclose(arquivoEntrada);
        return 3;
    }

    // Aloca memória para o vetor
    int *vetor = (int *)malloc(tamanho * sizeof(int));
    if (vetor == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        fclose(arquivoEntrada);
        return 4;
    }

    // Lê os elementos do vetor do arquivo
    if (fread(vetor, sizeof(int), tamanho, arquivoEntrada) != tamanho) {
        fprintf(stderr, "Erro ao ler os elementos do vetor do arquivo de entrada\n");
        fclose(arquivoEntrada);
        free(vetor);
        return 5;
    }

    // Fecha o arquivo de entrada
    fclose(arquivoEntrada);

    GET_TIME(inicio)

    // Ordena apenas a primeira metade do vetor
    long long int metade = tamanho / 2;
    merge_sort(vetor, 0, metade - 1);

    GET_TIME(fim)

    // Abre o arquivo para escrita binária
    FILE *arquivoSaida = fopen(nomeArquivoSaida, "wb");
    if (arquivoSaida == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de saída\n");
        free(vetor);
        return 6;
    }

    // Escreve o tamanho do vetor no arquivo de saída
    if (fwrite(&tamanho, sizeof(long long int), 1, arquivoSaida) != 1) {
        fprintf(stderr, "Erro ao escrever o tamanho do vetor no arquivo de saída\n");
        fclose(arquivoSaida);
        free(vetor);
        return 7;
    }

    // Escreve os elementos do vetor no arquivo de saída
    if (fwrite(vetor, sizeof(int), tamanho, arquivoSaida) != tamanho) {
        fprintf(stderr, "Erro ao escrever os elementos do vetor no arquivo de saída\n");
        fclose(arquivoSaida);
        free(vetor);
        return 8;
    }

    // Fecha o arquivo de saída
    fclose(arquivoSaida);

    // Libera a memória alocada
    free(vetor);

    printf("Primeira metade do vetor ordenada e salva com sucesso no arquivo %s\n", nomeArquivoSaida);
    
    delta = fim - inicio;
    printf("Tempo de ordenação: %lf\n", delta);
    
    return 0;
}
