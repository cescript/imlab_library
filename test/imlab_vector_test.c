#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "iocore.h"

typedef struct json_t {
    int type;
    void *value;
} json_t;

json_t jsonf(int type, float value) {
    json_t t;
    t.type = type;
    t.value = malloc(sizeof(float));
    memcpy(t.value, &value, sizeof(float));

    return t;
}

int main() {

    message(SUCCESS, "starting to test IMLAB vector library");

    vector_t *v = vector_create(json_t, 10);
    // create a temperory variables
    json_t t1 = jsonf(1, 2.3);

    for(int i=0; i < length(v); i++) {
        json_t d = at(json_t, v, i);

        if(d.type == 1) {
            printf("[%02d]: %d -- %3.2f\n", i, d.type, ((float*)d.value)[0]);
        } else if(d.type == 2) {
            printf("[%02d]: %d -- %d\n", i, d.type, ((int*)d.value)[0]);
        } else if(d.type == 3) {
            printf("[%02d]: %d -- %s\n", i, d.type, ((char*)d.value));
        }
    }
    // push 10000 elemenet to vector
    int N = 10000000;
    clock_t tstart = clock();
    for(int i=0; i < N; i++) {
        vector_push(v, &t1);
    }
    double tend = ((double)1000*(clock()-tstart)/CLOCKS_PER_SEC);
    printf("pushing %d element took %3.2f ms\n", N , tend);

    // test unique elements
    vector_t *in = vector_create(uint32_t);
    vector_t *un = vector_create(uint32_t);
    // fill the input vector
    for(int i=0; i < 100; i++) {
        int32_t r = random_int(10, 20) * 2;
        vector_push(in, &r);
    }
    // find the uinque elements
    vector_unique(in, un, NULL);
    // print the unique elements
    printf("unique elements are:\n");
    for(int i=0; i < length(un); i++) {
        printf("%d ", at(uint32_t, un, i));
    }

    message(SUCCESS, "all operations are done successfully!");
}
