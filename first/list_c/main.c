/**
 * implement a template linked list using macro
 */
#define TEMPLATE int
#include "list.h"
#undef TEMPLATE
#define TEMPLATE char
#include "list.h"
#undef TEMPLATE

#include <stdio.h>

int main() {
    while (1) {
        struct list_int l;
        struct list_char lc;
        list_init_int(&l);
        list_init_char(&lc);
        list_push_back_int(&l, 3);
        list_push_back_char(&lc, 'a');
//        printf("%d\n", list_pop_back_int(&l));
//        printf("%c\n", list_pop_back_char(&lc));
        list_destory_int(&l);
        list_destory_char(&lc);
    }
    return 0;
};
