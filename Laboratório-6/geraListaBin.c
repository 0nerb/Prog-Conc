#include <stdio.h>
#include <math.h>

int isPrime(int numero) {
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

int main(int argc, char *argv[]) {
  // Declaração de variáveis
  
  FILE *arquivo;

  // Entrada do limite superior
 long long int limite = atoll(argv[1]);
 long long int total = limite;

 long long int total_primos = 0;

  // Abertura do arquivo binário para escrita
  arquivo = fopen("numeros.bin", "wb");

  // Geração dos números inteiros e contagem de primos
  long long int i = 1;
    while (limite > 0) {
        fwrite(&i, sizeof(long long int), 1, arquivo);
        if (isPrime(i)) {
            printf("Primo: %lld\n", i); 
            total_primos++;
                
        }
        limite--;
        i++;
    }

  // Fechamento do arquivo binário
  fclose(arquivo);
  printf("Existe %lld primos em %lld números\n", total_primos, total);

return 0;
}


