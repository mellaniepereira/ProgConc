#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <math.h>

int M; //tamanho do buffer
int num_consumidores; //qnt de threads consumidoras
int *arr; //array auxiliar de numeros lidos no arquivo
long long int tam; //tamanho do array auxiliar
int *buffer; //buffer compartilhado
int num_primos = 0; //quantidade de números primos encontrados
int *num_primos_thread; //array da quantidade de primos encontrados por cada thread

sem_t slotCheio;  //semaforo para sincronizacao por condicao
sem_t slotVazio;  //semaforo para sincronizacao por condicao
sem_t mutex; //semaforo para sincronizacao entre produtores e consumidores

//função que verifica se um numero é primo
int ehPrimo(long long int n) {
  int i;
  if (n<=1) 
    return 0;
  if (n==2) 
    return 1;
  if (n%2==0)
    return 0;
  for (i=3; i<sqrt(n)+1; i+=2)
    if(n%i==0) 
      return 0;
  return 1;
}

//função auxiliar para preencher o array com os números do arquivo binário
void preencheArray(char * file) {
  FILE * descritorArquivo; //descritor do arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
  
  //abre o arquivo binario
  descritorArquivo = fopen(file, "rb");
  if(!descritorArquivo) {
    fprintf(stderr, "Erro de abertura do arquivo\n");
    exit(0);
  }

  //obtém o tamanho do arquivo
  ret = fread(&tam, sizeof(int), 1, descritorArquivo);
  if(!ret) {
    fprintf(stderr, "Erro de leitura das dimensoes do array\n");
    exit(0);
  }

  //aloca memoria para o array
  arr = (int*) malloc(sizeof(int) * tam);
  if(!arr) {
    fprintf(stderr, "Erro de alocao da memoria do array\n");
    exit(0);
  }

  //carrega o array de elementos do tipo int do arquivo
  ret = fread(arr, sizeof(int), tam, descritorArquivo);
  if(ret < tam) {
    printf("%ld",ret);
    fprintf(stderr, "Erro de leitura dos elementos do array\n");
    exit(0);
  }

  //finaliza o uso das variaveis
  fclose(descritorArquivo);
}

//função para inserir um elemento no buffer
void insere(int item) {
  static int in=0;
  sem_wait(&slotVazio); //aguarda slot vazio para inserir
  sem_wait(&mutex); //exclusao mutua 

  buffer[in] = item; 
  // printf("Inserido no buffer: %d\n", item);
  in = (in + 1) % M;

  sem_post(&mutex);
  sem_post(&slotCheio); //sinaliza um slot cheio
}

//funcao para retirar  um elemento no buffer
int retira() {
  int item;
  static int out=0;
  sem_wait(&slotCheio); //aguarda slot cheio para retirar
  sem_wait(&mutex); //exclusao mutua 

  item = buffer[out];
  // printf("Removido do buffer: %d\n", item);
  buffer[out] = 0;
  out = (out + 1) % M;

  sem_post(&mutex);
  sem_post(&slotVazio); //sinaliza um slot vazio

  return item;
}

//função do produtor 
void *produtor(void * arg) {
  free(arg);
  for (int i = 0; i < tam; i++) {
    insere(arr[i]);
  }
  for (int i = 0; i < M; i++) {
    insere(-1); //insere o valor de -1 para que as threads consumidoras saiam do loop
  }
  pthread_exit(NULL);
}

//função do consumidor
void *consumidor(void *arg) {
  int id = *(int *)arg;
  free(arg);
  int item;

  while (1) {
    item = retira();
    if (item == -1){ //todos os números foram processados
      insere(-1);    //inserindo final do arquivo
      break;
    }  
    if (ehPrimo(item)) {
      sem_wait(&mutex);
      num_primos++;
      sem_post(&mutex);
    }
  }
  //atualiza o array num_primos_thread com a contagem de primos da thread[id]
  num_primos_thread[id] = num_primos;
  // printf("thread[%d] tem %d primos\n", id, num_primos_thread[id]);

  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {

  //recebe e valida os parametros de entrada
  if(argc<4){
    fprintf(stderr, "Digite: %s <quantidade de threads consumidoras> <tamanho do buffer> <nome arquivo de entrada>", argv[0]);
    return 1;
  }
  
  num_consumidores = atoi(argv[1]);
  M = atoi(argv[2]);
  char *nome_arquivo = argv[3];

  //aloca memoria pro buffer compartilhado
  buffer = (int *) malloc(M * sizeof(int));
  if (buffer == NULL) {
    fprintf(stderr, "Erro ao alocar memória para o buffer");
    return 1;
  }

  //aloca memoria pro array de primos
  num_primos_thread = (int *)malloc(num_consumidores * sizeof(int));
  if (num_primos_thread == NULL) {
      fprintf(stderr, "Erro ao alocar memória para o array de primos");
      return 1;
  }

  //preenche o array com os numeros do arquivo binario
  preencheArray(nome_arquivo);
  
  //inicializa os semáforos
  sem_init(&mutex, 0, 1); //binario
  sem_init(&slotCheio, 0, 0); //contador
  sem_init(&slotVazio, 0, M); //contador

  //inicia as threads produtores
  pthread_t produtor_thread;
  if(pthread_create(&produtor_thread, NULL, produtor, NULL)) {
    fprintf(stderr, "Erro ao criar a thread produtora");
    return 1;
  }

  //inicia as threads consumidores
  pthread_t consumidores[num_consumidores];
  for (int i = 0; i < num_consumidores; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    if(pthread_create(&consumidores[i], NULL, consumidor, id)) {
      fprintf(stderr, "Erro ao criar a thread consumidora");
      return 1;
    }
  }

  //aguarda o término das threads
  pthread_join(produtor_thread, NULL);
  for (int i = 0; i < num_consumidores; i++) {
    pthread_join(consumidores[i], NULL);
  }

  //encontra a thread consumidora vencedora
  int thread_vencedora = 0; // inicialmente, consideramos a primeira thread como vencedora
  int max_primos_encontrados = num_primos_thread[0];

  for (int i = 1; i < num_consumidores; i++) {
    if (num_primos_thread[i] > max_primos_encontrados) {
      max_primos_encontrados = num_primos_thread[i];
      thread_vencedora = i;
    }
  }

  //imprime os resultados
  printf("Quantidade de números primos encontrados: %d\n", num_primos);
  printf("Thread consumidora vencedora: %d\n", thread_vencedora);

  //libera os semáforos
  sem_destroy(&slotCheio);
  sem_destroy(&slotVazio);
  sem_destroy(&mutex);

  //libera memória
  free(buffer);
  free(num_primos_thread);
  free(arr);

  return 0;
}