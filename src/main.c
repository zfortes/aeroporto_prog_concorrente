#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define MA 10
#define MB 10
#define GA 1
#define GB 1


void * comercialDecolando(void * meuid);
void * comercialPousando (void * meuid);
void * cargueiroDecolando (void * meuid);
void * cargueiroPousando (void * meuid);


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t comd_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t card_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t carp_cond = PTHREAD_COND_INITIALIZER;


// int mA,mB,gA,gB = 0;
int comD, comP, carD, carP = 0;

// int carD_Esperando = 0;
// int carP_Esperando = 0;

int carD_Esperando = 0;
int carP_Esperando = 0;

int bloqueio_pista = 0; 
int bloqueio_pista_decolagem = 0;
int bloqueio_pista_aterrisagem = 0;


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tP[MA];
  for (i = 0; i < MA; i++) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tP[i], NULL, comercialDecolando, (void *) (id));

    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }


  pthread_t tA[MB];
  for (i = 0; i < MB; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tA[i], NULL, comercialPousando, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  pthread_t tF[GA];
  for (i = 0; i < GA; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tF[i], NULL, cargueiroDecolando, (void *) (id));
    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }


 pthread_t tGB[GB];
  for (i = 0; i < GB; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tGB[i], NULL, cargueiroPousando, (void *) (id));
    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  pthread_join(tP[0],NULL);
}	

void * comercialDecolando(void * a){
  int i = *((int *) a);
  while(1){
    sleep(rand()%(i+1));
    pthread_mutex_lock(&mutex);
      if(comP > 0){
        bloqueio_pista = 0;
      }
      while(bloqueio_pista != 0 || carD_Esperando != 0 || carP_Esperando != 0 || comP > 0 || carD > 0 || carP > 0) {
        pthread_cond_wait(&comd_cond,&mutex);
      }

      comD++;
    pthread_mutex_unlock(&mutex);
    printf(" ^  Avião comercial %d decolando \n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
      comD--;
      printf(" -- Avião comercial %d decolou num: %d\n" ,i,comD);
      if(comD == 0){
        pthread_cond_broadcast(&comp_cond);
        pthread_cond_signal(&card_cond);
        pthread_cond_signal(&carp_cond);
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
      if(comD > 0){
          bloqueio_pista = 1;
      }
      while(bloqueio_pista != 1 || carD_Esperando != 0 || carP_Esperando != 0 || comD > 0 || carD > 0 || carP > 0) {
        pthread_cond_wait(&comp_cond,&mutex);
      }

	    comP++;
	  pthread_mutex_unlock(&mutex);
    printf(" v Avião comercial %d pousando \n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
	    comP--;
      printf(" _ Avião comercial %d pousou; num: %d\n" ,i,comP);
	    if(comP == 0){
        pthread_cond_broadcast(&comd_cond);
        pthread_cond_signal(&card_cond);
        pthread_cond_signal(&carp_cond);
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
        carD_Esperando++;
        while(comD > 0 || comP > 0 || carD > 0 || carP > 0) {
          pthread_cond_wait(&card_cond,&mutex);
        }
        carD_Esperando--;
        carD++;
      pthread_mutex_unlock(&mutex);
      printf(" ^  Cargueiro %d decolando \n",i);
      sleep(5);
      pthread_mutex_lock(&mutex);
        carD--;
        printf(" -- Cargueiro %d decolou; num: %d\n",i,carD);
        pthread_cond_broadcast(&comd_cond);
        pthread_cond_broadcast(&comp_cond);
        pthread_cond_signal(&card_cond);
        pthread_cond_signal(&carp_cond);
      pthread_mutex_unlock(&mutex);
    }
  pthread_exit(0);
}



void * cargueiroPousando(void * a){
  int i = *((int *) a);
  while(1){
    sleep(rand()%(i+1)+10);
    pthread_mutex_lock(&mutex);
      carP_Esperando++;
      while(comD > 0 || comP > 0 || carD > 0 || carP > 0) {
        pthread_cond_wait(&carp_cond,&mutex);
      }
      carP_Esperando--;
      carP++;
	  pthread_mutex_unlock(&mutex);
	  printf(" V  Cargueir %d pousando\n",i);
    sleep(5);
    pthread_mutex_lock(&mutex);
      carP--;
      printf(" _  Cargueiro %d pousou; num: %d\n" ,i,carP);
      pthread_cond_broadcast(&comd_cond);
      pthread_cond_broadcast(&comp_cond);
      pthread_cond_signal(&card_cond);
      pthread_cond_signal(&carp_cond);
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}
