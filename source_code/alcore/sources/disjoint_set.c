#include <stdlib.h>
#include "private/alcore_private.h"

struct disjoint_set_t *disjoint_set_create(uint32_t elements) {
    // create the output
    struct disjoint_set_t *out = (struct disjoint_set_t*) malloc(sizeof(struct disjoint_set_t));
    check_memory(out, NULL);
    // set the length of the vertices
    out->length = elements;
    // try to allocate the first edge list
    out->parent = (uint32_t *) malloc(out->length * sizeof(uint32_t));

    // try to allocate the label array
    out->label = (uint32_t *) malloc(out->length * sizeof(uint32_t));

    uint32_t i;
    for(i = 0; i < out->length; i++) {
        out->parent[i] = UINT32_MAX;
        out->label[i] = i;
    }
    out->rank = (int32_t *) calloc(out->length , sizeof(int32_t));

    // return the list
    return out;
}

// find the root of the
uint32_t disjoint_set_find(struct disjoint_set_t *set, uint32_t idxP) {
    // check the set is constructed
    check_null(set, idxP);
    // find the root of the idxP
    while(set->parent[idxP] != UINT32_MAX && idxP !=  set->parent[idxP]) {
        idxP =  set->parent[idxP];
    }
    // if the soure index is not same with the destination
    return idxP;
}

// implementation of the quick union find algorithm
return_t disjoint_set_union(struct disjoint_set_t *set, uint32_t idxP, uint32_t idxQ) {
    // check the set is constructed
    check_null(set, ERROR_NULL_TYPE);
    // check that the given two vertices are valid
    if(idxP >= set->length || idxQ >= set->length) {
        return ERROR_DIMENSION_MISMATCH;
    }

    // find the root of the two nodes
    uint32_t rP = disjoint_set_find(set, idxP);
    uint32_t rQ = disjoint_set_find(set, idxQ);

    // do nothing if the root of the two nodes are the same
    if(rP == rQ) {
        return WARNING_NOTHING_DONE;
    }

    // swap the ranks if necessary
    if(set->rank[rP] < set->rank[rQ]) {
        swap(uint32_t, rP, rQ);
    }
    // connect the two nodess
    set->parent[rQ] = rP;
    // increase the ranks
    if(set->rank[rP] == set->rank[rQ]) {
        set->rank[rP]++;
    }

    uint32_t parent;
    // compress the path from idxQ to rQ
    while( (parent = set->parent[idxQ]) != UINT32_MAX )
    {
        set->parent[idxQ] = rP;
        idxQ = parent;
    }

    // compress the path from idxP to rP
    while( (parent = set->parent[idxP]) != UINT32_MAX )
    {
        set->parent[idxP] = rP;
        idxP = parent;
    }
    return SUCCESS;
}

// enumerate the roots
uint32_t disjoint_set_enumerate(struct disjoint_set_t *set) {

    check_null(set, 0);

    uint32_t i;
    uint32_t nclasses = 0;

    // set the ranks to zero
    for( i = 0; i < set->length; i++ ) {
        set->rank[i] = 0;
    }

    for( i = 0; i < set->length; i++ )
    {
        // get the root of the i
        uint32_t rI = disjoint_set_find(set, i);
        // set the rank to negative value if the current root is untouched
        if( set->rank[rI] == 0 ) {
            set->rank[rI] = ~(nclasses++);
        }
        // assign the label
        set->label[i] = (uint32_t )(~set->rank[rI]);
    }

    return nclasses;
}

void disjoint_set_free(struct disjoint_set_t **set) {
    // return if the set is empty
    check_null(set[0], );

    // free the allocated memory
    free(set[0]->rank);
    free(set[0]->parent);
    free(set[0]->label);
    // free the set
    free(set[0]);
}

void disjoint_set_view(struct disjoint_set_t *set) {

    check_null(set, );

    uint32_t i;
    printf("Index \t Parent \t Label\n");
    // print all the elements in tree
    for(i = 0; i < set->length; i++) {
        uint32_t parent = set->parent[i] != UINT32_MAX ? set->parent[i] : i;
        printf("%05d: \t %05d \t (%05d)\n", i, parent, set->label[i]);
    }
}
