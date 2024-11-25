#include <stdio.h>

#include "vmlib.h"

int main()
{
    vminit(2000); // comment this out if using vmload()

    /* alternatively, load a heap image */
    // vmload("tests/img/many_free.img");

    void *ptr = vmalloc(16);
    void *ptr1 = vmalloc(100);
    void *ptr2 = vmalloc(20);
    // void *pt3 = vmalloc(30);
    // void *ptr4 = vmalloc(1500);

    vminfo(); // print out how the heap looks like at this point in time for
              // easy visualization

    vmdestroy(); // frees all memory allocated by vminit() or vmload()
    return 0;
}
