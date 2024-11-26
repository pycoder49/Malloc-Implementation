#include <stdio.h>

#include "vmlib.h"

int main()
{
    vminit(2000); // comment this out if using vmload()

    /* alternatively, load a heap image */
    // vmload("tests/img/many_free.img");

    //void *ptr = vmalloc(16);
    void *ptr = vmalloc(16);
    void *ptr2 = vmalloc(32);
    void *ptr3 = vmalloc(100);
    void *ptr4 = vmalloc(200);
    void *ptr5 = vmalloc(4);
    
    vminfo(); // print out how the heap looks like at this point in time for
              // easy visualization
    
    printf("\n\n\n");


    vmfree(ptr2);
    vmfree(ptr3);
    vminfo();


    printf("\n\n\n");


    vmfree(ptr2);
    vminfo();


    vmdestroy(); // frees all memory allocated by vminit() or vmload()
    return 0;
}
