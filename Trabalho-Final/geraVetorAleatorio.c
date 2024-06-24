#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void gerarVetorAleatorio(int *vetor, long long int tamanho) {
    for (long long int i = 0; i < tamanho; i++) {
        vetor[i] = rand() % 10;  // Valores aleatórios entre 0 e 999
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <tamanho do vetor> <nome do arquivo>\n", argv[0]);
        return 1;
    }

    long long int tamanho = atoll(argv[1]);
    char *nomeArquivo = argv[2];

    // Aloca memória para o vetor
    int *vetor = (int *)malloc(tamanho * sizeof(int));
    if (vetor == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 2;
    }

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Gera o vetor aleatório
    gerarVetorAleatorio(vetor, tamanho);

    // Abre o arquivo para escrita binária
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para escrita\n");
        free(vetor);
        return 3;
    }

    // Escreve o tamanho do vetor no arquivo
    if (fwrite(&tamanho, sizeof(long long int), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao escrever o tamanho do vetor no arquivo\n");
        fclose(arquivo);
        free(vetor);
        return 4;
    }

    // Escreve os elementos do vetor no arquivo
    if (fwrite(vetor, sizeof(int), tamanho, arquivo) != tamanho) {
        fprintf(stderr, "Erro ao escrever os elementos do vetor no arquivo\n");
        fclose(arquivo);
        free(vetor);
        return 5;
    }

    // Fecha o arquivo
    fclose(arquivo);

    // Libera a memória alocada
    free(vetor);

    printf("Vetor gerado e salvo com sucesso no arquivo %s\n", nomeArquivo);

    return 0;
}
