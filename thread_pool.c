//
// Created by Nicolas A Perez on 3/5/18.
//

#include <stdlib.h>
#include <ntsid.h>
#include <pthread.h>
#include <stdbool.h>
#include "thread_pool.h"

/**
 * The pool represents the p_id that are available to do work.
 * The p_id is NOT a pthread_t, but instead, the id given to thread by the main thread main.c/init_threads function.
 * */
int *pool;
int pool_size;

/**
 * pool_lock makes the thread pool thread safe.
 * */
pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;

void init_pool(int size) {
    pool_size = size;

    pool = calloc((size_t) pool_size, sizeof(int));

    //there is no one on the pool at the start point.
    for (int i = 0; i < pool_size; ++i) {
        pool[i] = 0;
    }
}

void free_pool() {
    free(pool);
}

/**
 * add_to_pool adds (logically) the thread with id p_id to the pool.
 * */
void add_to_pool(int p_id) {
    pthread_mutex_lock(&pool_lock);

    pool[p_id] = 1;

    pthread_mutex_unlock(&pool_lock);
}

/**
 * remove_from_pool removes (logically) the thread with id p_id from the pool.
 * */
void remove_from_pool(int p_id) {
    pool[p_id] = 0;
}

/**
 * get_from_pool returns a p_id that is ready to work.
 * There are two ways to select an available p_id. Randomly seems perform better and avoids starvation.
 * This function blocks until some thread becomes available.
 * */
int get_from_pool() {

    int result_id;

    while (true) {
        pthread_mutex_lock(&pool_lock);

        // select a random thread and check if it is available
        int idx = rand() % pool_size;

        if (pool[idx] > 0) {
            result_id = idx;
            remove_from_pool(idx); // thread i is not available any longer (some work will be assign to it)

            pthread_mutex_unlock(&pool_lock);
            return result_id;
        }

//        this select the first available thread (can cause starvation of threads with high ids)
//        for (int i = 0; i < pool_size; ++i) {
//            if (pool[i] > 0) {
//                result_id = i;
//                remove_from_pool(i); // thread i is not available any longer (some work will be assign to it)
//
//                pthread_mutex_unlock(&pool_lock);
//                return result_id;
//            }
//        }

        pthread_mutex_unlock(&pool_lock);
    }
}

/**
 * This waits until all thread become available on the pool
 * */
void wait_for_all_in_pool() {
    int count = 0;

    while (count < pool_size) {
        count = 0;

        pthread_mutex_lock(&pool_lock);

        for (int i = 0; i < pool_size; ++i) {
            if (pool[i] > 0) {
                count++;
            }
        }

        pthread_mutex_unlock(&pool_lock);
    }
}
