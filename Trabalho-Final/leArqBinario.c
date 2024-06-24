#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome do arquivo>\n", argv[0]);
        return 1;
    }

    char *nomeArquivo = argv[1];

    // Abre o arquivo para leitura binária
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para leitura\n");
        return 2;
    }

    // Lê o tamanho do vetor do arquivo
    long long int tamanho;
    if (fread(&tamanho, sizeof(long long int), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao ler o tamanho do vetor do arquivo\n");
        fclose(arquivo);
        return 3;
    }

    // Aloca memória para o vetor
    int *vetor = (int *)malloc(tamanho * sizeof(int));
    if (vetor == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        fclose(arquivo);
        return 4;
    }

    // Lê os elementos do vetor do arquivo
    if (fread(vetor, sizeof(int), tamanho, arquivo) != tamanho) {
        fprintf(stderr, "Erro ao ler os elementos do vetor do arquivo\n");
        fclose(arquivo);
        free(vetor);
        return 5;
    }

    // Fecha o arquivo
    fclose(arquivo);

    // Imprime os elementos do vetor no terminal
    for (long long int i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

    // Libera a memória alocada
    free(vetor);

    return 0;
}