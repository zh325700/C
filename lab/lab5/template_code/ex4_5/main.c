#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"
#include "time.h"

dplist_t * list = NULL;

int dplist_errno;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void dpl_print(dplist_t * list);
void dpl_print_reverse(dplist_t * list);

void dpl_cleanup_rand(dplist_t * list);

typedef struct {
    int id;
    char* name;
} my_element_t;

int main(void) {
    my_element_t x;
    char* name;
    asprintf(&name,"Jeroen");
    x.name = name;
    

    list = dpl_create(element_copy, element_free, element_compare);
    assert(dplist_errno == 0);

    printf("Adding 3 elements to the list\n");
    x.id = 1;
    list = dpl_insert_at_index(list, &x, 0, true);
    assert(dplist_errno == 0);

    x.id = 3;
    list = dpl_insert_at_index(list, &x, 1, true);
    assert(dplist_errno == 0);

    x.id = 7;
    list = dpl_insert_at_index(list, &x, 5, true);
    assert(dplist_errno == 0);

    printf("list size = %d\n", dpl_size(list));

    printf("Adding one extra element\n");
    x.id = 5;
    list = dpl_insert_at_index(list, &x, 2, true);
    assert(dplist_errno == 0);

    dpl_print(list);

    printf("Adding 4 more elements to the list (sorted)\n");
    x.id = 9;
    list = dpl_insert_sorted(list, &x, true);
    assert(dplist_errno == 0);

    x.id = 4;
    list = dpl_insert_sorted(list, &x, true);
    assert(dplist_errno == 0);

    x.id = 5;
    list = dpl_insert_sorted(list, &x, true);
    assert(dplist_errno == 0);

    x.id = 0;
    list = dpl_insert_sorted(list, &x, true);
    assert(dplist_errno == 0);

    dpl_print(list);

    x.id = 4;
    printf("element '%d' has index = %d\n", x.id, dpl_get_index_of_element(list, &x));

    printf("Removing one element from the list\n");
    list = dpl_remove_element(list, &x, true);
    assert(dplist_errno == 0);

    dpl_print(list);

    dpl_print_reverse(list);

    dpl_print(list);

    printf("Calling all search methods and inserting an extra element with copy\n");
    dplist_node_t* node = dpl_get_first_reference(list);
    node = dpl_get_next_reference(list, node);
    node = dpl_get_reference_of_element(list, &x);
    dpl_insert_at_reference(list, &x, node, true);
    dpl_remove_at_reference(list, node, true);

    //dpl_cleanup(list);
    printf("Removing all elements from the list\n");
    dpl_cleanup_rand(list);

    dpl_print_reverse(list);

    printf("Free the list list\n");
    dpl_free(&list, true);
    assert(dplist_errno == 0);

    free(name);
    return 0;
}

//void * element_copy(void * element) {
//    int * copy = malloc(sizeof (int));
//    assert(copy != NULL);
//    *copy = *(int *) element;
//    return (void *) copy;
//}

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

//void element_free(void ** element) {
//    free(*element);
//    *element = NULL;
//}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

//int element_compare(void * x, void * y) {
//    return ((*(int *) x < *(int *) y) ? -1 : (*(int *) x == *(int *) y) ? 0 : 1);
//}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

//void dpl_print(dplist_t * list) {
//    int i, length;
//    length = dpl_size(list);
//    assert(dplist_errno == 0);
//    printf("\nList =");
//    for (i = 0; i < length; i++) {
//        int * element;
//        element = dpl_get_element_at_index(list, i);
//        assert(dplist_errno == 0);
//        printf(" %d", *element);
//    }
//    printf("\n");
//}

void dpl_print(dplist_t * list) {
    int i, length;
    length = dpl_size(list);
    assert(dplist_errno == 0);
    printf("\nList =");
    for (i = 0; i < length; i++) {
        my_element_t* element;
        element = dpl_get_element_at_index(list, i);
        assert(dplist_errno == 0);
        printf(" %d", element->id);
    }
    printf("\n");
}

//void dpl_print_reverse(dplist_t * list) {
//    dplist_node_t * dummy;
//    dummy = dpl_get_last_reference(list);
//    printf("\nReverse list =");
//    while (dummy) {
//        int* element;
//        element = dpl_get_element_at_reference(list, dummy);
//        assert(dplist_errno == 0);
//        printf(" %d", *element);
//        dummy = dpl_get_previous_reference(list, dummy);
//        assert(dplist_errno == 0);
//    }
//    printf("\n");
//}

void dpl_print_reverse(dplist_t * list) {
    dplist_node_t * dummy;
    dummy = dpl_get_last_reference(list);
    printf("\nReverse list =");
    while (dummy) {
        my_element_t* element;
        element = dpl_get_element_at_reference(list, dummy);
        assert(dplist_errno == 0);
        printf(" %d", element->id);
        dummy = dpl_get_previous_reference(list, dummy);
        assert(dplist_errno == 0);
    }
    printf("\n");
}

void dpl_cleanup_rand(dplist_t * list) {
    int size;
    srand(time(NULL));

    do {
        int index;
        dplist_node_t * dummy;
        int * element;
        size = dpl_size(list);
        index = rand() % (size + 2) - 1;
        dummy = dpl_get_reference_at_index(list, index);
        assert(dplist_errno == 0);
        element = dpl_get_element_at_reference(list, dummy);
        assert(dplist_errno == 0);
        list = dpl_remove_element(list, element, true);
        assert(dplist_errno == 0);
    } while (size > 0);
}
