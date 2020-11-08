#include <stdlib.h>
#include "private/alcore_private.h"

// swap bytes of the list elements
void memswap(uint8_t * aptr, uint8_t * bptr, size_t size)
{
    while (size-- > 0) { uint8_t t = *aptr; *aptr++ = *bptr; *bptr++ = t; }
}

void quicksort(void* listdata, size_t length, size_t element_size, int (*compare)(void* context, const void* a, const void* b), void* context)
{
    if (listdata == NULL || length < 2 || element_size <= 0 || compare == NULL)
    {
        // print error
        return;
    }

    uint8_t *start = listdata;
	uint8_t *pivot = listdata;
	size_t  left = 0;
	size_t  right = element_size * (length - 1);

	while (left < right) 
    {
		while (compare(context, start+left, pivot) < 0 && left < right) 
        {
			left += element_size;
		}
		while (compare(context, start+right, pivot) > 0 && left < right) 
        {
			right -= element_size;
		}

		if (left < right && compare(context, start+left, start+right) != 0) 
        {
			// choose the pivot
			if (start+left == pivot) 
            {
				pivot = start+right;
			} 
            else if (start+right == pivot) 
            {
				pivot = start+left;
			}
			memswap(start+left, start+right, element_size);
		} 
        else 
        {
			left += element_size;
		}
	}

    size_t remaining = left / element_size;
	quicksort(start, remaining-1, element_size, compare, context);
	quicksort(start+left, (length-remaining), element_size, compare, context);
}
