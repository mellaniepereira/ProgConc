#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include "timer.h"

float *matriz1; //matriz de entrada
float *matriz2; //matriz de entrada
float *saida; //matriz de saida
int L, N, C; //linha da matriz1, (coluna da matriz1/linha da matriz2), coluna da matriz2
double inicio, fim, delta; //indicadores para os tempos

int contador = 0;
void preencheMatriz(float ** matriz, char* file) {
  FILE * descritorArquivo; //descritor do arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
  int linhas, colunas;
  long long int tam;
  
  //abre o arquivo binario
  descritorArquivo = fopen(file, "rb");
  if(!descritorArquivo) {
    fprintf(stderr, "Erro de abertura do arquivo\n");
    exit(0);
  }

  //le as dimensoes da matriz
  ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
  if(!ret) {
    fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
    exit(0);
  }
  ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
  if(!ret) {
    fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
    exit(0);
  }

  if(contador==0) {
    L = linhas;
    N = colunas;
  } else {
    //checa se o numero de colunas da matriz1 eh igual ao numero de linhas da matriz2
    if(N != linhas){
      fprintf(stderr, "Para multiplicar matrizes o numero de linhas da 1° deve ser igual ao numero de colunas da 2° matriz \n");
      exit(0);
    }
    C = colunas;
  }
  tam = linhas * colunas; //calcula a qtde de elementos da matriz

  //aloca memoria para a matriz
  *matriz = (float*) malloc(sizeof(float) * tam);
  if(!matriz) {
    fprintf(stderr, "Erro de alocao da memoria da matriz\n");
    exit(0);
  }

  //carrega a matriz de elementos do tipo float do arquivo
  ret = fread(*matriz, sizeof(float), tam, descritorArquivo);
  if(ret < tam) {
    fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
    exit(0);
  }

  //finaliza o uso das variaveis
  fclose(descritorArquivo);

  //incrementa o contador para preencher L,N,C
  contador ++;
}

void exportaMatriz(char * file) {
  FILE * descritorArquivo; //descritor do arquivo de saida
  size_t ret; //retorno da funcao de escrita no arquivo de saida

  //abre o arquivo para escrita binaria
  descritorArquivo = fopen(file, "wb");
  if(!descritorArquivo) {
    fprintf(stderr, "Erro de abertura do arquivo\n");
    exit(0);
  }

  //escreve numero de linhas e de colunas
  ret = fwrite(&L, sizeof(int), 1, descritorArquivo);
  ret = fwrite(&C, sizeof(int), 1, descritorArquivo);

  //escreve os elementos da matriz
  ret = fwrite(saida, sizeof(float), L*C, descritorArquivo);
  if(ret < L*C) {
    fprintf(stderr, "Erro de escrita no  arquivo\n");
    exit(0);
  }

  //finaliza o uso das variaveis
  fclose(descritorArquivo);
}

void multiplicaMatriz() {
  GET_TIME(inicio);
  //faz a multiplicacao
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < C; j++) {
      saida[i * C + j] = 0;
      for (int k = 0; k < N; k++) {
        saida[i * C + j] += matriz1[i * N + k] * matriz2[k * C + j];
      }
    }
  }
  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo sequencial: %lf\n", delta);
}

int main(int argc, char* argv[]) {
  //recebe e valida os parametros de entrada
  if(argc<3){
    fprintf(stderr, "Digite: %s <arquivo entrada1> <arquivo entrada2> <arquivo saida>", argv[0]);
    return 1;
  }

  //preenche as matrizes de entrada
  preencheMatriz(&matriz1, argv[1]);
  preencheMatriz(&matriz2, argv[2]);

  //alocacao da memoria para matriz resultado
  saida = (float *) malloc(sizeof(float) * L * C); 
  if (saida == NULL) {
    printf("Erro: não foi possível alocar memória para a matriz de saída.\n");
    exit(0);
  }

  //calcula a multiplicacao de matrizes
  multiplicaMatriz();

  //escreve a matriz de saida no arquivo binario
  exportaMatriz(argv[3]);

  //libera a memoria alocada
  free(matriz1);
  free(matriz2);
  free(saida);

  return 0;
}