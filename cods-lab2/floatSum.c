#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

long int dim; // dimensao do vetor de entrada
int nthreads; // numero de threads
float *vec; // vetor de entrada

//fluxo das threads
void * task(void * arg) {
  long int id = (long int) arg; // identificador da thread
  float *localSum;  // variavel local de soma
  long int blockSize = dim / nthreads;  // tamanho do bloco de cada thread
  long int start = id * blockSize; // elemento inicial do bloco da thread
  localSum = (float*) malloc(sizeof(float));
  if(localSum == NULL){
    fprintf(stderr, "Error--malloc\n");
    exit(1);
  }
  *localSum = 0;
  long int end;  // elemento final do bloco da thread
  if(id == nthreads - 1){ // caso seja a ultima thread
    end = dim; // trata o resto se houver
  }
  else{
    end = start + blockSize; 
  }
  // soma os elementos do bloco
  for(long int i = start; i < end; i++) {
    *localSum += vec[i];
  }
  // retorna o resultado da soma local
  pthread_exit((void *) localSum);
}

//fluxo principal 
int main(int argc, char *argv[]) {
  float seqSum = 0; // soma sequencial
  float concSum = 0; // soma concorrente
  pthread_t *tid; // identificador da thread no sistema
  float *response; // valor de resposta das threads

  // recebe e valida o parametro de entrada <numero de threads>
  if(argc < 2) {
    fprintf(stderr, "Enter: %s <number of threads> \n", argv[0]);
    return 1;
  }
  
  nthreads = atoi(argv[1]);
  scanf("%ld", &dim); // pega o primeiro elemento do arquivo.txt gerado por gera_vet_rand.c

  // aloca o vetor de entrada 
  vec = (float*) malloc(sizeof(float) * dim);
  if(vec == NULL) {
    fprintf(stderr, "Error--malloc\n");
    return 2;
  }
  
  // preenche o vetor de entrada com os valores do arquivo.txt
  for(long int i = 0; i < dim; i++) {
    scanf("%f", &vec[i]);
  }

  // soma sequencial dos elementos 
  for(long int i = 0; i < dim; i++) {
    seqSum += vec[i];
  }

  // soma concorrente dos elementos 
  tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid == NULL) {
    fprintf(stderr, "Error--malloc\n");
    return 2;
  }

  // cria as threads
  for(long int i = 0; i < nthreads; i++) {
    if(pthread_create(tid + i, NULL, task, (void*) i)){
      fprintf(stderr, "Error--pthread_create\n");
      return 3;
    }
  }

  // aguarda o termino das threads 
  for(long int i = 0; i < nthreads; i++) {
    if(pthread_join(*(tid + i), (void**) &response)){
      fprintf(stderr, "Error--pthread_join\n");
      return 4;
    }
    concSum += *response;
    free(response); 
  }

  // resultados
  printf("Soma sequencial: %f\n", seqSum);
  printf("Soma concorrente: %f\n", concSum);

  // libera os espaços de memoria alocadas
  free(vec);
  free(tid);

  return 0;
}