#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(int argc, char* argv[]) {
  int *sequencia; //sequência de números inteiros que será gerada
  long long int n; //quantidade de números inteiros a serem gerados
  FILE *descritorArquivo; //descritor do arquivo de saída
  size_t ret; //retorno da função de escrita no arquivo de saída
    
  //recebe os argumentos de entrada
  if(argc < 3) {
    fprintf(stderr, "Digite: %s <quantidade> <arquivo saída>\n", argv[0]);
    return 1;
  }
  n = atoll(argv[1]); //quantidade de números inteiros
  
  //aloca memória para a sequência
  sequencia = (int*) malloc(sizeof(int) * n);
  if(!sequencia) {
    fprintf(stderr, "Erro de alocação de memória\n");
    return 2;
  }

  //preenche a sequência com valores inteiros aleatórios
  srand(time(NULL));
  for(long long int i = 0; i < n; i++) {
    sequencia[i] = rand();
  }

  //escreve a sequência no arquivo
  //abre o arquivo para escrita binária
  descritorArquivo = fopen(argv[2], "wb");
  if(!descritorArquivo) {
    fprintf(stderr, "Erro de abertura do arquivo\n");
    free(sequencia);
    return 3;
  }

  //escreve a dimensao do vetor
  ret = fwrite(&n, sizeof(int), 1, descritorArquivo);

  //escreve os elementos da sequência
  ret = fwrite(sequencia, sizeof(int), n, descritorArquivo);
  if(ret < n) {
    fprintf(stderr, "Erro de escrita no arquivo\n");
    fclose(descritorArquivo);
    free(sequencia);
    return 4;
  }

  //finaliza o uso das variáveis
  fclose(descritorArquivo);
  free(sequencia);
  return 0;
}