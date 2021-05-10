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



// void * comercialDecolando(void * meuid);
// void * comercialPousando (void * meuid);
// void * cargueiroDecolando (void * meuid);
// void * cargueiroPousando (void * meuid);


// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t comd_cond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t comp_cond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t card_cond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t carp_cond = PTHREAD_COND_INITIALIZER;







int mA,mB,gA,gB = 0;

int gaQuer = 0;
int gbQuer = 0;

int bloqueio_pista = 0; 

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
      if(mB > 0){
        bloqueio_pista = 0;
      }
      while(bloqueio_pista != 0 || gaQuer != 0 || gbQuer != 0 || mB > 0 || gA > 0 || gB > 0) {
        pthread_cond_wait(&comd_cond,&mutex);
      }

      mA++;
    pthread_mutex_unlock(&mutex);
    printf("Avião comercial %d decolando \n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
      mA--;
      printf("Avião comercial %d decolou num: %d\n" ,i,mA);
      if(mA == 0){
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
      if(mA > 0){
          bloqueio_pista = 1;
      }
      while(bloqueio_pista != 1 || gaQuer != 0 || gbQuer != 0 || mA > 0 || gA > 0 || gB > 0) {
        pthread_cond_wait(&comp_cond,&mutex);
      }

	    mB++;
	  pthread_mutex_unlock(&mutex);
    printf("Avião comercial %d pousando \n",i);
    sleep(1);
    pthread_mutex_lock(&mutex);
	    mB--;
      printf("Avião comercial %d pousou; num: %d\n" ,i,mB);
	    if(mB == 0){
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
        gaQuer++;
              while(mA > 0 || mB > 0 || gA > 0 || gB > 0) {
            pthread_cond_wait(&card_cond,&mutex);
            }
              gaQuer--;
              gA++;
      pthread_mutex_unlock(&mutex);
      printf("Cargueiro %d decolando \n",i);
      sleep(5);
      pthread_mutex_lock(&mutex);
      gA--;
      printf("Cargueiro %d pousando; num: %d\n",i,gA);
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
      gbQuer++;
      while(mA > 0 || mB > 0 || gA > 0 || gB > 0) {
        pthread_cond_wait(&carp_cond,&mutex);
      }
      gbQuer--;
      gB++;
	  pthread_mutex_unlock(&mutex);
	  printf("Cargueir %d pousando\n",i);
    sleep(5);
    pthread_mutex_lock(&mutex);
      gB--;
      printf("Cargueiro %d pousou; num: %d\n" ,i,gB);
      pthread_cond_broadcast(&comd_cond);
      pthread_cond_broadcast(&comp_cond);
      pthread_cond_signal(&card_cond);
      pthread_cond_signal(&carp_cond);
    pthread_mutex_unlock(&mutex);
    }
  pthread_exit(0);
}
