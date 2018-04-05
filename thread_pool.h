//
// Created by Nicolas A Perez on 3/5/18.
//

#ifndef HOMEWORK3_THREAD_POOL_H
#define HOMEWORK3_THREAD_POOL_H

void add_to_pool(int p_id);

void init_pool(int size);

void free_pool();

int get_from_pool();

void wait_for_all_in_pool();

#endif //HOMEWORK3_THREAD_POOL_H
