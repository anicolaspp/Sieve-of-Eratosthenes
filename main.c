#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <memory.h>
#include "thread_pool.h"
#include "structures.h"


int next_value(int i, int n);
void read_args(int argc, char *const *argv, int *p, int *c);
void init_numbers(int n);
void init_thread_data(int p);
void init_threads(int p);
void master_run(int c);
void *slave_run(void *p_id);
void filter_data_segment(int local_id);
void print_primes_when_debug();
void print_result();
void clean_thread_data();
void clean_numbers();

struct Data_t *data;
pthread_mutex_t data_lock = PTHREAD_MUTEX_INITIALIZER;

int *numbers;
int n;

bool DEBUG_ENABLED = false;


/*
 * 1 - When increasing the number of threads to be used, the size c should also be increased.
 * 2 - A high number of threads and a small block size is actually worst than small number of threads.
 * 3 - To aximize performance, increase the number of threads and increase the block size, this reduce context switching
 *      and the application behave way better.
 * 4 - You can pass -d as fifth argument to show the process log (DEBUG_ENABLED)
 * */
int main(int argc, char *argv[]) {
    int p;
    int c;

    read_args(argc, argv, &p, &c);

    init_numbers(n);
    init_pool(p);
    init_thread_data(p);
    init_threads(p);

    master_run(c);

    print_primes_when_debug();

    print_result();

    clean_thread_data();
    free_pool();
    clean_numbers();

    return 0;
}

int next_value(int i, int n) {
    for (int j = i + 1; j < sqrt(n); ++j) {
        if (numbers[j] > 0) {
            return numbers[j];
        }
    }

    return -1;
}

void read_args(int argc, char *const *argv, int *p, int *c) {

    if (argc < 4) {
        printf("required arguments: [n - the max number we want to find out about prime numbers]\n [p - number of slave threads]\n[c - chunk size]\n");
        exit(1);
    }

    (*p) = atoi(argv[2]);
    (*c) = atoi(argv[3]);
    n = atoi(argv[1]);

    n++;

    if (argc == 5) {
        char *debug = argv[4];

        if (strcmp(debug, "-d") == 0) {
            DEBUG_ENABLED = true;
        }
    }
}

void init_numbers(int n) {
    numbers = calloc((size_t) n, sizeof(int));

    for (int i = 0; i < n; ++i) {
        numbers[i] = i;
    }
}

void init_thread_data(int p) {
    data = calloc((size_t) p, sizeof(data_t));

    for (int i = 0; i < p; ++i) {
        data[i].value = -1;

        pthread_cond_init(&data[i].ready, NULL);
    }
}

void init_threads(int p) {
    for (int i = 0; i < p; ++i) {
        pthread_t pthread;

        pthread_create(&pthread, NULL, (void *) &slave_run, (void *) i);
        add_to_pool(i); // the thread i has become available on the pool
    }
}

void master_run(int c) {
    int k = 2;

    while (true) {
        if (k == -1) {
            break;
        }

        for (int j = 0; j <= n; j += c) { // foreach block
            int p_id = get_from_pool(); //blocks until a thread becomes available. It takes the thread out of the pool.

            pthread_mutex_lock(&data_lock);

            data[p_id].min = j;
            data[p_id].size = c;
            data[p_id].value = k;
            pthread_cond_signal(&data[p_id].ready); // we signal thread with id p_id to start working.

            pthread_mutex_unlock(&data_lock);
        }

        // Wait until all blocks have been process so we can find the next value of K.
        // This is required to avoid miss calculation of the value K.
        // If we don't wait, we could be selecting a K value that can be removed by a thread that has not finished.
        wait_for_all_in_pool();

        k = next_value(k, n);
    }
}

void *slave_run(void *p_id) {
    int local_id = (int) p_id;

    while (true) { // thread life-cycle.

        while (data[local_id].value < 0) {
            pthread_cond_wait(&data[local_id].ready, &data_lock); // waits for data to be available for p_id
        }

        // there is some work for me
        filter_data_segment(local_id);

        add_to_pool(local_id); // I am done with the filtering, add me back to the pool so I am ready to take more work.
    }
}

void filter_data_segment(int local_id) {

    struct Data_t local_data = data[local_id];

    if (DEBUG_ENABLED) {
        printf("[thread_id = %d] -- filtering value = %d in segment [%d, %d]\n",
               local_id,
               local_data.value,
               local_data.min,
               local_data.min + local_data.size);
    }

    int i = local_data.min;

    while (i < local_data.min + local_data.size && i <= n) {
        if (numbers[i] != local_data.value && numbers[i] % local_data.value == 0) {
            // we don't need a lock in here since the segment [ local_data.min, local_data.min + local_data.size ]
            // is exclusively accessed by one thread at a time.
            numbers[i] = -1;
        }

        i++;
    }

    // we don't need a lock in here since data[local_id] is exclusively accessed by thread with id local_id
    data[local_id].value = -1;
}

void print_primes_when_debug() {
    if (DEBUG_ENABLED) {
        printf("[");

        for (int i = 0; i < n; ++i) {
            if (numbers[i] > 0) {
                printf("%d ", numbers[i]);
            }
        }

        printf("]\n");
    }
}

void print_result() {
    printf("\n");

    if (numbers[n - 1] >= 0) {
        printf("%d is prime\n", n - 1);
    } else {
        printf("%d is NOT prime\n", n - 1);
    }
}

void clean_thread_data() { free(data); }

void clean_numbers() { free(numbers); }
