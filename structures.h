//
// Created by Nicolas A Perez on 3/5/18.
//

#ifndef HOMEWORK3_STRUCTURES_H
#define HOMEWORK3_STRUCTURES_H

struct Data_t {

    int min;
    int size;
    int value;
    pthread_cond_t ready;

} data_t;


#endif //HOMEWORK3_STRUCTURES_H
