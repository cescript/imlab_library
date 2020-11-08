//
// Created by cescript on 21.06.2018.
//

#ifndef IMLAB_ALCORE_HPP
#define IMLAB_ALCORE_HPP

#include <stdint.h>
#include "core.h"

// graph structure adjacency list
struct disjoint_set_t 
{
    uint32_t length; // number of edges in the graph
    uint32_t *parent; // parent
    uint32_t *label; // label of each element
    // private member
    int32_t *rank;
};

/**
 * Create a disjoint set data structure
 * @param length Length of the set
 * @return A disjoint set data structure
 */
struct disjoint_set_t *disjoint_set_create(uint32_t length);

/**
 * Fınds the root of the disjoint set element
 * @param set Disjoint set data structure
 * @param idxP Index of the element
 * @return Root of the given element
 */
uint32_t disjoint_set_find(struct disjoint_set_t *set, uint32_t idxP);

/**
 * Unions two element and updates the disjoint set tree
 * @param set Disjoint set data structure
 * @param idxP Index of the first element
 * @param idxQ Index of the second element
 * @return Success or relative error
 */
return_t disjoint_set_union(struct disjoint_set_t *set, uint32_t idxP, uint32_t idxQ);

/**
 * Enumerate the roots of each element
 * @param set Disjoint set data structure
 * @return Number of unique root elements
 */
uint32_t disjoint_set_enumerate(struct disjoint_set_t *set);

/**
 * Free the memory allocated by the disjoint set data structure
 * @param set Disjoint set data structure
 */
void disjoint_set_free(struct disjoint_set_t **set);

/**
 * Prints the current status of the disjoint set tree
 * @param set
 */
void disjoint_set_view(struct disjoint_set_t *set);

/**
 * Implementation of the quick sort algorithm.
 * This algorithm is efficient for large arrays
 * @param listdata Input list to be sorted, content of the pointer will be will change after sort operation
 * @param length Length of the input list
 * @param element_size Size of the single element in the list
 * @param compare A comparison function which gives positive values for a > b, negative values for a < b and 0 for not cares
 * @param context Context pointer is passed directly to the compare function. This allows the compare function to access any data outside of its scope
 */
void quicksort(void* listdata, size_t length, size_t element_size, int (*compare)(void* context, const void* a, const void* b), void* context);


#endif //IMLAB_ALCORE_HPP
