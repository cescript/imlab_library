#include <string.h> // for memcpy
#include <stdlib.h> // for malloc vs
#include "private/core_private.h"

struct array_header_t {
    uint32_t length[4];
};

// return the head (starting position of the real data)
void *array_head(struct array_header_t *origin) {
    return origin == NULL ? NULL : ((unsigned char*)origin + sizeof(struct array_header_t));
}
// return the origin (starting of the fat array)
void *array_origin(void *head) {
    return head == NULL ? NULL : ((unsigned char*)head - sizeof(struct array_header_t));
}

// create a new array and allocate the pointers
void *array_create_1dim(uint32_t _n1length, size_t t1) {

    // do logical checks
    check_condition(_n1length > 0, "length of the array must be bigger than zero", NULL);
    //printf("size: %d %d %d %d | %d %d %d %d\n", _n1length, 0, 0, 0, t1, 0, 0, 0);
    // allocate the memory for the first dimension
    struct array_header_t *origin = calloc(1, _n1length * t1 + sizeof(struct array_header_t));

    // fill the header of the array
    origin->length[0] = _n1length;
    origin->length[1] = 0;
    origin->length[2] = 0;
    origin->length[3] = 0;

    // return the output
    return array_head(origin);
}

// create a new array and allocate the pointers
void *array_create_2dim(uint32_t _n1length, uint32_t _n2length, size_t t1, size_t t2) {

    // do logical checks
    check_condition(_n2length > 0 && _n1length > 0, "lengths of the array must be bigger than zero", NULL);
    //printf("size: %d %d %d %d | %d %d %d %d\n", _n1length, _n2length, 0, 0, t1, t2, 0, 0);
    struct array_header_t *origin = calloc(1, _n1length * t1 + sizeof(struct array_header_t));

    // try to allocate the other dimension, if the first is allocated
    if(origin != NULL) {

        // check that the memory is allocated
        uint32_t is_memory_status_ok = 1;

        // fill the header of the array
        origin->length[0] = 0; // these will be used as a loop counter
        origin->length[1] = _n2length;
        origin->length[2] = 0;
        origin->length[3] = 0;

        // assuming that the head is 4 dimensional fat array
        void **head = array_head(origin);

        // free each dim individually
        for(origin->length[0] = 0; (origin->length[0] < _n1length) && is_memory_status_ok; origin->length[0]++) {

            // try to allocate the data[i]'s
            head[origin->length[0]] = calloc(_n2length, t2);
            is_memory_status_ok = (head[origin->length[0]] != NULL ? 1:0);
        }

        // if any of the fields are not allocated, return null
        if(is_memory_status_ok == 0) {
            // clear the origin of the array (this function will find the origin from the head)
            array_free(head);
            // set it to NULL
            origin = NULL;
        }
    }
    // return the output
    return array_head(origin);
}

// create a new array and allocate the pointers
void *array_create_3dim(uint32_t _n1length, uint32_t _n2length, uint32_t _n3length, size_t t1, size_t t2, size_t t3) {

    // do logical checks
    check_condition(_n3length > 0 && _n2length > 0 && _n1length > 0, "lengths of the array must be bigger than zero", NULL);
    //printf("size: %d %d %d %d | %d %d %d %d\n", _n1length, _n2length, _n3length, 0, t1, t2, t3, 0);
    struct array_header_t *origin = calloc(1, _n1length * t1 + sizeof(struct array_header_t));

    // try to allocate the other dimension, if the first is allocated
    if(origin != NULL) {

        // check that the memory is allocated
        uint32_t is_memory_status_ok = 1;

        // fill the header of the array
        origin->length[0] = 0; // these will be used as a loop counter
        origin->length[1] = 0; // these will be used as a loop counter
        origin->length[2] = _n3length;
        origin->length[3] = 0;

        // assuming that the head is 4 dimensional fat array
        void ***head = array_head(origin);

        // free each dim individually
        for(origin->length[0] = 0; (origin->length[0] < _n1length) && is_memory_status_ok; origin->length[0]++) {

            // try to allocate the data[i]'s
            head[origin->length[0]] = calloc(_n2length, t2);
            is_memory_status_ok = (head[origin->length[0]] != NULL ? 1:0);

            for(origin->length[1] = 0; (origin->length[1] < _n2length) && is_memory_status_ok; origin->length[1]++) {

                // try to allocate the data[i][j]'s
                head[origin->length[0]][origin->length[1]] = calloc(_n3length, t3);
                is_memory_status_ok = (head[origin->length[0]][origin->length[1]] != NULL ? 1:0);

            }
        }

        // if any of the fields are not allocated, return null
        if(is_memory_status_ok == 0) {
            // clear the origin of the array (this function will find the origin from the head)
            array_free(head);
            // set it to NULL
            origin = NULL;
        }
    }
    // return the output
    return array_head(origin);
}

// create a new array and allocate the pointers
void *array_create_4dim(uint32_t _n1length, uint32_t _n2length, uint32_t _n3length, uint32_t _n4length, size_t t1, size_t t2, size_t t3, size_t t4) {

    // do logical checks
    check_condition(_n4length > 0 && _n3length > 0 && _n2length > 0 && _n1length > 0, "lengths of the array must be bigger than zero", NULL);
    //printf("size: %d %d %d %d | %d %d %d %d\n", _n1length, _n2length, _n3length, _n4length, t1, t2, t3, t4);
    struct array_header_t *origin = calloc(1, _n1length * t1 + sizeof(struct array_header_t));

    // try to allocate the other dimension, if the first is allocated
    if(origin != NULL) {

        // check that the memory is allocated
        uint32_t is_memory_status_ok = 1;

        // fill the header of the array
        origin->length[0] = 0; // these will be used as a loop counter
        origin->length[1] = 0; // these will be used as a loop counter
        origin->length[2] = 0; // these will be used as a loop counter
        origin->length[3] = _n4length;

        // assuming that the head is 4 dimensional fat array
        void ****head = array_head(origin);

        // free each dim individually
        for(origin->length[0] = 0; (origin->length[0] < _n1length) && is_memory_status_ok; origin->length[0]++) {

            // try to allocate the data[i]'s
            head[origin->length[0]] = calloc(_n2length, t2);
            is_memory_status_ok = (head[origin->length[0]] != NULL ? 1:0);

            for(origin->length[1] = 0; (origin->length[1] < _n2length) && is_memory_status_ok; origin->length[1]++) {

                // try to allocate the data[i][j]'s
                head[origin->length[0]][origin->length[1]] = calloc(_n3length, t3);
                is_memory_status_ok = (head[origin->length[0]][origin->length[1]] != NULL ? 1:0);

                for(origin->length[2] = 0; (origin->length[2] < _n3length) && is_memory_status_ok; origin->length[2]++) {

                    // try to allocate the data[i][j][k]'s
                    head[origin->length[0]][origin->length[1]][origin->length[2]] = calloc(_n4length, t4);
                    is_memory_status_ok = (head[origin->length[0]][origin->length[1]][origin->length[2]] != NULL ? 1:0);

                }
            }
        }

        // if any of the fields are not allocated, return null
        if(is_memory_status_ok == 0) {
            // clear the origin of the array (this function will find the origin from the head)
            array_free(head);
            // set it to NULL
            origin = NULL;
        }
    }
    // return the output
    return array_head(origin);
}

// return the size of the array in the asked dimension
uint32_t array_size(void *head, uint32_t dim) {
    struct array_header_t *origin = array_origin(head);
    if(origin == NULL || dim > 4) {
        return 0;
    }
    return origin->length[dim];
}


void array_free_2dim(void **head, uint32_t n1length) {
    uint32_t i;
    // free each dim individually
    for(i = 0; i < n1length; i++) {
        free(head[i]);
    }
}

void array_free_3dim(void ***head, uint32_t n1length, uint32_t n2length) {
    uint32_t i, j;
    // free each dim individually
    for(i = 0; i < n1length; i++) {
        // free the third dim
        for(j = 0; j < n2length; j++) {
            free(head[i][j]);
        }
        // free the second dim
        free(head[i]);
    }
}

void array_free_4dim(void ****head, uint32_t n1length, uint32_t n2length, uint32_t n3length) {
    uint32_t i, j, k;
    // free each dim individually
    for(i = 0; i < n1length; i++) {
        // free the third dim
        for(j = 0; j < n2length; j++) {
            // free the fourth dimension
            for(k = 0; k < n3length; k++) {
                free(head[i][j][k]);
            }
            free(head[i][j]);
        }
        // free the second dim
        free(head[i]);
    }
}

void array_free(void *head) {

    struct array_header_t *origin = array_origin(head);
    if(origin == NULL) {
        return;
    }

    // free array dimensions first
    if(origin->length[3] > 0) {
        array_free_4dim(head, origin->length[0], origin->length[1], origin->length[2]);
    }
    else if(origin->length[2] > 0) {
        array_free_3dim(head, origin->length[0], origin->length[1]);
    }
    else if(origin->length[1] > 0) {
        array_free_2dim(head, origin->length[0]);
    }
    // free the fat array
    free(origin);
}