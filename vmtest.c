#include <stdio.h>

#include "vmlib.h"

int main()
{
    vminit(2000); // comment this out if using vmload()

    /* alternatively, load a heap image */
    // vmload("tests/img/many_free.img");

    //void *ptr = vmalloc(16);
    void *ptr0 = vmalloc(1024);
    void *ptr1 = vmalloc(256);
    void *ptr2 = vmalloc(256);
    void *ptr3 = vmalloc(256);
    void *ptr4 = vmalloc(256);
    void *ptr5 = vmalloc(256);
    void *ptr6 = vmalloc(256);
    void *ptr7 = vmalloc(256);
    void *ptr8 = vmalloc(256);
    void *ptr9 = vmalloc(256);
    void *ptr10 = vmalloc(256);
    void *ptr11 = vmalloc(256);
    void *ptr12 = vmalloc(256);
    void *ptr13 = vmalloc(256);
    void *ptr14 = vmalloc(256);
    void *ptr15 = vmalloc(256);

    vminfo(); // print out how the heap looks like at this point in time for
              // easy visualization

    vmdestroy(); // frees all memory allocated by vminit() or vmload()
    return 0;
}
