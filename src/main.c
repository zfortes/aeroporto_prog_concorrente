#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Quantidade de voos comerciais e cargueiros que iram pousar ou decolar
#define COMERCIALD 10
#define COMERCIALP 10
#define CARGEIROD 1
#define CARGEIROP 1


void * comercialDecolando(void * meuid);
void * comercialPousando (void * meuid);
void * cargueiroDecolando (void * meuid);
void * cargueiroPousando (void * meuid);


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t comd_cond = PTHREAD_COND_INITIALIZER; // condicao do voo comercial que esta decolando
pthread_cond_t comp_cond = PTHREAD_COND_INITIALIZER; // condicao do voo comercial que esta pousando
pthread_cond_t card_cond = PTHREAD_COND_INITIALIZER; // condicao do voo cargueiro que esta decolando
pthread_cond_t carp_cond = PTHREAD_COND_INITIALIZER; // condicao do voo cargueiro que esta pousando

int comD, comP, carD, carP = 0;

int carD_Esperando = 0;
int carP_Esperando = 0;

void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t comerciaisD[COMERCIALD];
  for (i = 0; i < COMERCIALD; i++) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&comerciaisD[i], 0, comercialDecolando, (void *) (id));

    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }


  pthread_t comerciaisP[COMERCIALP];
  for (i = 0; i < COMERCIALP; i++) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&comerciaisP[i], 0, comercialPousando, (void *) (id));

    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t cargueirosD[CARGEIROD];
  for (i = 0; i < CARGEIROD; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&cargueirosD[i], 0, cargueiroDecolando, (void *) (id));
    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t cargueirosP[CARGEIROP];
  for (i = 0; i < CARGEIROP; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&cargueirosP[i], 0, cargueiroPousando, (void *) (id));
    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  
  pthread_join(comerciaisD[0],NULL);
}	

void * comercialDecolando(void * a){
  int i = *((int *) a);
  while(1){
    sleep(rand()%(i+1));
    pthread_mutex_lock(&mutex); // bloqueia o mutex
      while(carP_Esperando > 0 || carD_Esperando > 0 || comD > 0 || carD > 0 || carP > 0) { // condicao que verifica se existe algum cargueiro esperando
        pthread_cond_wait(&comd_cond,&mutex);                                               // ou se a pista de decolagem ja esta sendo utilizada
      }
      comD++; // informa que um aviao esta indo decolar
    pthread_mutex_unlock(&mutex);
    printf(" D ?  Avião comercial decolando %d \n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
      comD--; // seta como esse aviao ja tendo decolado
      printf(" D + Avião comercial decolou    %d\n" ,i);
      if(comD == 0){
        pthread_cond_signal(&comd_cond); // acorda um voo vomercial que esteja esperando para decolar
        pthread_cond_signal(&card_cond); // acorda um voo cargueiro que esteja esperando para decolar
        pthread_cond_signal(&carp_cond); // acorda um voo cargueiro que esteja esperando para pousar
      }
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}

void * comercialPousando(void * a){
  int i = *((int *) a);
  while(1){
    sleep(rand()%(i+1));
    pthread_mutex_lock(&mutex);
      while(carP_Esperando > 0 || carD_Esperando > 0 || comP > 0 || carD > 0 || carP > 0) { // condicao que verifica se existe algum cargueiro esperando
        pthread_cond_wait(&comp_cond,&mutex);                                               // ou se a pista de aterrisagem ja esta sendo utilizada
      }
	    comP++;
	  pthread_mutex_unlock(&mutex);
    printf(" P ? Avião comercial pousando     %d\n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
	    comP--;
      printf(" P + Avião comercial pousou       %d\n" ,i);
	    if(comP == 0){
        pthread_cond_signal(&comp_cond); // acorda um voo vomercial que esteja esperando para pousar
        pthread_cond_signal(&card_cond); // acorda um voo cargueiro que esteja esperando para decolar
        pthread_cond_signal(&carp_cond); // acorda um voo cargueiro que esteja esperando para pousar
      }
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}


void * cargueiroDecolando(void * a){
    int i = *((int *) a);
    while(1){
      sleep(rand()%(i+1)+2);
      pthread_mutex_lock(&mutex);
        carD_Esperando++; // sinaliza que existe um voo cargueiro esperando para decolar
        while(comD > 0 || comP > 0 || carD > 0 || carP > 0) { // condicao que verifica se a pista ja esta em uso por alguem
          pthread_cond_wait(&card_cond,&mutex);
        }
        carD_Esperando--;
        carD++; // sinaliza q existe um voo cargueiro decolando
      pthread_mutex_unlock(&mutex);
      printf("    D ? Cargueiro %d decolando \n",i);
      sleep(5);
      pthread_mutex_lock(&mutex);
        carD--;
        printf("    D + Cargueiro %d decolou \n",i);
        pthread_cond_signal(&comd_cond); // acorda um voo comercial que esteja esperando para decolar
        pthread_cond_signal(&comp_cond); // acorda um voo comercial que esteja esperando para pousar
        pthread_cond_signal(&card_cond); // acorda um voo cargueiro que esteja esperando para decolar 
        pthread_cond_signal(&carp_cond); // acorda um voo cargueiro que esteja esperando para pousar
      pthread_mutex_unlock(&mutex);
    }
  pthread_exit(0);
}



void * cargueiroPousando(void * a){
  int i = *((int *) a);
  while(1){
    sleep(rand()%(i+1)+10);
    pthread_mutex_lock(&mutex);
      carP_Esperando++; // sinaliza que existe um voo cargueiro esperando para pousar
      while(comD > 0 || comP > 0 || carD > 0 || carP > 0) {
        pthread_cond_wait(&carp_cond,&mutex);
      }
      carP_Esperando--;
      carP++; // sinaliza q existe um voo cargueiro pousando
	  pthread_mutex_unlock(&mutex);
	  printf("    P ? Cargueiro %d pousando\n",i);
    sleep(5);
    pthread_mutex_lock(&mutex);
      carP--;
      printf("    P + Cargueiro %d pousou \n" ,i);
      pthread_cond_signal(&comd_cond); // acorda um voo comercial que esteja esperando para decolar
      pthread_cond_signal(&comp_cond); // acorda um voo comercial que esteja esperando para pousar
      pthread_cond_signal(&card_cond); // acorda um voo cargueiro que esteja esperando para decolar
      pthread_cond_signal(&carp_cond); // acorda um voo cargueiro que esteja esperando para pousar
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}
