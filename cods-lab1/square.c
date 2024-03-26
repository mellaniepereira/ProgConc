#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS 2
#define N_ELEMENTS 100

//vetor de 1000 elementos
int vec[N_ELEMENTS];

int div_elem = N_ELEMENTS/NTHREADS;

//funcao para calcular o quadrado dos elementos
void * square(void * arg) {
  int *vector = (int *) arg;
  for(int i=0; i<div_elem; i++) {
    vector[i] *= vector[i];
  }
  pthread_exit(NULL);
}

void check_square() {
  for(int number=0; number<N_ELEMENTS; number++) {
    if(vec[number] != (number*number))
      printf("Erro! O calculo do vetor ao quadrado esta errado.\n");
  };
  printf("Sucesso! O calculo do vetor ao quadrado esta certo!\n");
}

int main() {
  int pid=0;  //incrementador de contador da thread
  pthread_t tid[NTHREADS]; //identificador da thread no sistema

  //cria um array com N_ELEMENTS
  for(int i=0; i<N_ELEMENTS; i++) {
    vec[i] = i;
  };
  
  //cria NTHREADS
  for(int start=0; start < N_ELEMENTS; start = (start + div_elem)) {
    if(pthread_create(&tid[pid], NULL, square, (void *)&vec[start])) {
      printf("Erro ao criar a thread: %d\n", pid);
    }
    pid++;
  }
  for(int i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("Erro de join");
    }
  };
  //for(int i=0; i<N_ELEMENTS; i++) {printf("%d ", vec[i]);}  //para visualizar o vetor final
  //confere se os valores estão corretos 
  check_square();
  pthread_exit(NULL);
  return 0;
}