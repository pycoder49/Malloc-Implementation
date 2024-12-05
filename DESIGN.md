1) Give an example of a program (in C or pseudocode) that sets up a situation where a 20-byte vmalloc call fails to allocate despite the heap having (in total) over 200 bytes free across many blocks. Assume all the policies and layout of the allocator from the PA are used (best fit, alignment, coalescing rules, and so on)


Pseudocode:


init_heap(1000)

void *ptr1 = vmalloc(8);     --> each call allocates 16 bytes for alignmnent purposes
void *ptr2 = vmalloc(8);
void *ptr3 = vmalloc(8);
...
keep vmallocing(8) until you can not anymore

when you can not vmalloc anymore, start freeing every other block:

vmfree(ptr1);
vmfree(ptr3);
vmfree(ptr5);
...
keep vmfreeing(ptr__)

This makes it so that roughly half of the bytes are free, roughly 500 in our case.

However, each freed block is only 16 bytes of freed memory, which is too small for our vmalloc(20) call.

Because of this, this call will fail and NULL will be returned, even through there are well over 200 free total bytes of memory within out heap.





2) Give an example of a program (in C or pseudocode) where all the allocations succeed if using the best fit allocation policy (like in this PA), but some allocations would fail due to fragmentation if the first fit allocation policy was used instead, keeping everything else the same.


Best-fit policy selects the smallest block that is large enough to fit the requested size rounded to a multiple of 16.

First-fit policy selects the first block it encounters and check's if it's large enough to fit the rounded size, if not, return null;


Let's say we have the following program:

ptr1 = vmalloc(16) --> rounds up to 32 bytes
ptr2 = vmalloc(40) --> rounds up to 48 bytes
ptr3 = vmalloc(24) --> rounsd up to 32 bytes

vmfree(ptr2)    --> frees the 48 bytes in heap

ptr4 = vmalloc(8)   --> rounds up to 16 bytes
ptr5 = vmalloc(40)  --> rounds up to 48 bytes again



end heap structure after using best-fit
[32    ][16   ][32     ][32    ][48      ][remaining free       ]       --> rough heap structure
            |   |
            |   |-> 32 free bytes
            |
            |-> 48 bytes free split into a 16 and 32 when calling vmalloc(8)


end heap structure after using first-fit
[32    ][16   ][32    ][32    ][remaining free       ]       --> rough heap structure
                |
                |-> 32 free bytes

As we can see, the first-fit approach fails to properly allocate vmalloc(40) because we need a block of 48 bytes, but since the 
first empty block is a 32 byte long block, it simply returns null, never going to the end of the heap like best-fit and 
allocating to the end of the heap.





-------------------------------------------------------------------
-------------------------------------------------------------------
UPDATED DESIGN QUESTION
-------------------------------------------------------------------
-------------------------------------------------------------------


1) Consider an updated implementation where in case of freeing, we only coalesce it with exactly 1 next block in the heap 
if it is free. Give an example of a program (in C or pseudocode) where all the allocations succeed in the current design
(like in this PA), but some allocations would fail with the updated freeing strategy.


Pseudocode:

init_heap(200);

//allocate blocks
ptr1 = vmalloc(20); --> allocates a 32-byte block
ptr2 = vmalloc(20);
ptr3 = vmalloc(20);
ptr4 = vmalloc(20);

//now we free some blocks
vmfree(ptr2);
vmfree(ptr4);
vmfree(ptr3);

//now we allocate a larger block
ptr5 = vmalloc(70);



This vmalloc call fails and returns NULL because after all the vmalloc() and free() calls, there are a total
of two free blocks--> ptr2 (free with 32 bytes), and ptr3 (free with 64 bytes).

When ptr3 was freed, in our updated logic, only the next block will be joined together with it (ptr4 which is 
32 bytes) and not the previous block (ptr2 which is 32 free bytes). So, after freeing, ptr3 now points to a block
with 64 bytes of free memory.

Now, if you try to vmalloc(70), it will fail because the program will not be able to find a block large enough,
even though there are over 70 bytes of free memory right next to each other.
