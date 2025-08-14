// simular um token ring
// T generico numero de threads
// token começa com 0
// criar T-1 threads para incrementar a variavel token
// T = 4 -> termina com a thread0 incrementando o token 
// mais uma vez ate token = 4


#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

#define T 50

/* t0 libera a t1, que libera a t2, .... t n-1 libera a t0
main inicializa todas as threads e imprime

todos os semaphores sao inicializados em 1
[0, 1, 2, 3, 4, 5, ..., T]
[1, 1, 1, 1, 1, 1, ..., 1]

main inicia T-1 threads, em que cada thread espera pelo seu seu respectivo sempahore 
obs: (main é semaphore[0])

T1 espera por sempahore[1]
T2 espera por sempahore[2]
..
Tn-1 espera por semaphore[T-1]

apos liberadas, as threads incrementam o token & libera o semaphore[i+1]
obs: exceto a ultima, que libera o sempahore[0] (o qual a main espera)

apos todas as inicializaçoes, main libera o semaphore[1] e espera o sempahore[0], e a magica acontece

nao é necessário join, pois todas as threads morrem sozinhas e a main obrigatoriamente
espera pela ultima pela modelagem da soluçao

apos tudo , main incrementa o token uma ultima vez e printa o resultadoi

*/  

sem_t semaphores[T];
int token = 0;

void incrementer(void *inc_id) {
    int *inc = (int*) inc_id;

    // esperar por sempahoro inc_id
    sem_wait(&(semaphores[*inc]));
    token++;

    // liberar semaphoro[inc+1] (caso nao seja o ultimo)
    if (*inc < T-1) sem_post(&(semaphores[*inc +1]));
    else sem_post(&(semaphores[0])); // libera o primeiro (T0)

    free (inc_id); // variavel dinamicamente alocada
}   

int main() {

    pthread_t handlers[T];
    

    for (int i = 0; i < T ; i++) {
        sem_init(&(semaphores[i]), 0, 1); // todos os semaphores começam em 1

        int * incrementer_id = malloc(sizeof(int));
        if (incrementer_id == NULL) {
            printf("erro ao alocar incrementer id");
            exit(EXIT_FAILURE);
        }   
        
        *incrementer_id = i+1;

        if (pthread_create(&(handlers[i]), 0, (void*) incrementer, incrementer_id) != 0) {
            printf("error ao criar thread: %d", *incrementer_id);
            exit(EXIT_FAILURE);
        }
    }

    if (T>0 )sem_post(&(semaphores[1]));

    sem_wait(&(semaphores[0])); // esperar para a thread 0
    token++;

    printf("Incremento completo na T0. Token: %d\n", token);
}
