#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void memswap(unsigned char* a, unsigned char* b, size_t size)
{
	unsigned char tmp;
	for ( size_t i = 0; i < size; i++ )
	{
		tmp = a[i];
		a[i] = b[i];
		b[i] = tmp;
	}
}

static
unsigned char* array_index(unsigned char* base,
                           size_t element_size,
                           size_t index)
{
	return base + element_size * index;
}

static
size_t partition(unsigned char* base,
                 size_t element_size,
                 size_t num_elements,
                 size_t pivot_index,
                 int (*compare)(const void*, const void*, void*),
                 void* arg)
{
	if ( pivot_index != num_elements - 1 )
	{
		unsigned char* pivot = array_index(base, element_size, pivot_index);
		unsigned char* other = array_index(base, element_size, num_elements - 1);
		memswap(pivot, other, element_size);
		pivot_index = num_elements - 1;
	}

	size_t store_index = 0;
	for ( size_t i = 0; i < num_elements - 1; i++ )
	{
		unsigned char* pivot = array_index(base, element_size, pivot_index);
		unsigned char* value = array_index(base, element_size, i);
		if ( compare(value, pivot, arg) <= 0 )
		{
			unsigned char* other = array_index(base, element_size, store_index);
			if ( value != other )
				memswap(value, other, element_size);
			store_index++;
		}
	}

	unsigned char* pivot = array_index(base, element_size, pivot_index);
	unsigned char* value = array_index(base, element_size, store_index);
	memswap(pivot, value, element_size);

	return store_index;
}

static void _qsort_r(void* base_ptr,
             size_t num_elements,
             size_t element_size,
             int (*compare)(const void*, const void*, void*),
             void* arg)
{
	unsigned char* base = (unsigned char*) base_ptr;

	if ( !element_size || num_elements < 2 )
		return;

	size_t pivot_index = num_elements / 2;
	pivot_index = partition(base, element_size, num_elements, pivot_index, compare, arg);

	if ( 2 <= pivot_index )
		_qsort_r(base, pivot_index, element_size, compare, arg);

	if ( 2 <= num_elements - (pivot_index + 1) )
		_qsort_r(array_index(base, element_size, pivot_index + 1),
		        num_elements - (pivot_index + 1), element_size, compare, arg);
}

static int compare_wrapper(const void* a, const void* b, void* arg)
{
	return ((int (*)(const void*, const void*)) arg)(a, b);
}

void sortix_qsort(void* base_ptr,
           size_t num_elements,
           size_t element_size,
           int (*compare)(const void*, const void*))
{
	_qsort_r(base_ptr, num_elements, element_size, compare_wrapper, (void*) compare);
}
