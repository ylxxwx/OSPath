#include "ordered_array.h"
#include "mem.h"

s8 standard_lessthan_predicate(type_t a, type_t b) {
    return (a < b)? 1:0;
}

ordered_array_t create_ordered_array(u32 max_size, lessthan_predicate_t less_than) {
    u32 addr = kmalloc_nofree(max_size * sizeof(type_t));
    return place_ordered_array(addr, max_size, less_than);
}

ordered_array_t place_ordered_array(void *addr, u32 max_size, lessthan_predicate_t less_than) {
    ordered_array_t ret;
    ret.array = (type_t*) addr;
    kmemset(ret.array, 0, max_size * sizeof(type_t));
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;
    return ret;
}

void destroy_ordered_array(ordered_array_t *array) {
    // the memory is not freeable.
}

void insert_ordered_array(type_t item, ordered_array_t *array) {
    //ASSERT(array->less_than);
    if (array->size == array->max_size) {
        kprintf("ordered array is full.\n");
        return;
    }
    u32 iterator = 0;
    while(iterator < array->size && array->less_than(array->array[iterator], item)) {
        iterator++;
    }
    if (iterator == array->size) {
        array->array[array->size++] = item; 
    } else {
        type_t tmp = array->array[iterator];
        array->array[iterator] = item;
        while (iterator < array->size) {
            iterator++;
            type_t tmp2 = array->array[iterator];
            array->array[iterator] = tmp;
            tmp = tmp2;
        }
        array->size++;
    }
}

type_t lookup_ordered_array(u32 i, ordered_array_t *array) {
    return array->array[i];
}

void remove_ordered_array(u32 i, ordered_array_t *array) {
    while (i < array->size) {
        array->array[i] = array->array[i+1];
        i++;
    }
    array->size--;
}
