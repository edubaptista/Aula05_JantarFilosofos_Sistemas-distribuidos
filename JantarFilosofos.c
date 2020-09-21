/* 1. Each philosopher runs as a separate thread. */
/* 2. Philosophers alternate between thinking and eating. */
/* 3. To simulate both activities, have the thread sleep for a random period between one and three seconds. */
/* 4. Invoke `pickup_forks()` when wishing to eat; `return_forks()` on finishing. */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N_PHILOSOPHERS 10
#define LEFT (ph_num + N_PHILOSOPHERS - 1) % N_PHILOSOPHERS
#define RIGHT (ph_num + 1) % N_PHILOSOPHERS

#define tPensando 0.1
#define tComendo 0.1

pthread_mutex_t mutex;
pthread_cond_t condition[N_PHILOSOPHERS];

int cycle = 0;

enum { THINKING, HUNGRY, EATING } state[N_PHILOSOPHERS];
int phil_num[N_PHILOSOPHERS];

void *philosophing (void *arg);
void pickup_forks(int ph_num);
void return_forks(int ph_num);
void test(int ph_num);

int main(int argc, char *argv[])
{
  /* Setup */
  pthread_t ph_thread[N_PHILOSOPHERS];
  pthread_mutex_init(&mutex, NULL);

  int i;
  for (i = 0; i < N_PHILOSOPHERS; i++) {
    pthread_cond_init(&condition[i], NULL);
    phil_num[i] = i;		
  }

  /* Meat */

  for (i = 0; i < N_PHILOSOPHERS; i++) {
    pthread_create(&ph_thread[i], NULL, philosophing, &phil_num[i]);
    cycle += 1;
    printf("Cycle %d - Philosopher #%d sits on the table.\n", cycle, i + 1);
    sleep(1);
  }
  for (i = 0; i < N_PHILOSOPHERS; i++)
    pthread_join(ph_thread[i], NULL);

  /* Cleanup */
  pthread_mutex_destroy(&mutex);
  for (i = 0; i < N_PHILOSOPHERS; i++)
    pthread_cond_destroy(&condition[i]);

  return(0);
}

void *philosophing(void *arg)
{
  while(1) {
    int *ph_num = arg;
    cycle += 1;
    printf("Cycle %d - Philosopher #%d starts thinking.\n", cycle, *ph_num + 1);
    sleep(tPensando);
    pickup_forks(*ph_num);
    return_forks(*ph_num);
  }
}

void pickup_forks(int ph_num) {
  //pthread_mutex_lock(&mutex);
  cycle += 1;
  printf("Cycle %d - Philosopher #%d is HUNGRY. She tries to grab her forks.\n", cycle, ph_num + 1);
  state[ph_num] = HUNGRY;
  test(ph_num);
  while (state[ph_num] != EATING) 
  	pthread_cond_wait(&condition[ph_num], &mutex);

   //pthread_mutex_unlock(&mutex);
}

void return_forks(int ph_num) {
  pthread_mutex_lock(&mutex);

  cycle += 1;
  printf("Cycle %d - Philosopher #%d puts down chopsticks. Now she asks her neighbors if they are hungry.\n", cycle, ph_num + 1);

  state[ph_num] = THINKING;
  test(LEFT);
  test(RIGHT);

  pthread_mutex_unlock(&mutex);
}

void test(int ph_num) {
  if (state[ph_num] == HUNGRY && 
      state[LEFT] != EATING && 
      state[RIGHT] != EATING) {
    cycle += 1;
    printf("Cycle %d - Philosopher #%d starts EATING.\n", cycle, ph_num + 1);
    state[ph_num] = EATING;
    sleep(tComendo);
    pthread_cond_signal(&condition[ph_num]);
  }
}

/* reference: https://c-program-example.com/2012/02/c-program-to-solve-dining-philosophers-problem.html */
