#include "vm.h"
#include "vmlib.h"
#include <stdio.h>
#include <assert.h>

/**
 * HELPER FUNCTIONS:
 * 
 * int get_rounded(int size)
 * @return needed total bytes for the vmalloc call
 * 
 * int _size_in_bytes(struct block_header*)
 * @return size of current block in bytes
 * 
 * int _size_in_8byte_words(struct block_header*)
 * @return number of 8 bytes in current block
 * 
 * void* _get_next_block(struct block_header*)
 * @return posinter to the next block after current
 * 
 * void perform_split(struct block_header*, int)
 * splits the big block into two, one busy and one free
 * @return nothing
 */

static int _get_rounded(int size){
    return ROUND_UP(size + 8, BLKALIGN);
}

static int _get_size_in_bytes(struct block_header* current_pointer){
    return BLKSZ(current_pointer);
}

// static int _get_size_in_8byte_words(struct block_header* current_pointer){
//     return BLKSZ(current_pointer) / 8;
// }

static void* _get_next_block(struct block_header* current_pointer){
    return (struct block_header *)((char *)current_pointer + _get_size_in_bytes(current_pointer));
}

static void _perform_split(struct block_header* current_pointer, int requested_size) {
    int rounded_size = _get_rounded(requested_size);
    int size_of_original_block = _get_size_in_bytes(current_pointer);

    printf("\n\n\n\n");

    //first we get the total size of the first block
    printf("requested size: %d\n", requested_size);
    printf("rounded size: %d\n", rounded_size);
    printf("size in bytes of current at start: %d\n", _get_size_in_bytes(current_pointer));

    printf("--------------------\n");
    printf("Total size of current and second blocks after split: %d %d\n",
            rounded_size, _get_size_in_bytes(current_pointer) - rounded_size);
    printf("--------------------\n");

    //updating information about the first block
    struct block_header* first_block = current_pointer;

    first_block->size_status = rounded_size | (first_block->size_status & VM_PREVBUSY) | VM_BUSY;
    printf("size of  first block: %lu\n", (unsigned long)(first_block->size_status & VM_BLKSZMASK));
    //first_block->size_status &= VM_ENDMARK;


    //adding the second block header
    struct block_header* second_block = (struct block_header *)((char *)current_pointer + rounded_size);

    second_block->size_status = (size_of_original_block - _get_size_in_bytes(first_block)) |
                                VM_PREVBUSY;
    second_block->size_status &= ~VM_BUSY;



    
    printf("\n\n\n\n");
    
    
    
    
    
    
    
    
    
    
    
    
    // //calculate the second block pointer
    // struct block_header* second_block = (struct block_header *)ROUND_UP(
    //     (uintptr_t)((char *)current_pointer + rounded_size), BLKALIGN
    // );
    // int second_block_size = _get_size_in_bytes(current_pointer) - (int)((char *)second_block - (char *)current_pointer);

    // //set second block header
    // second_block->size_status = second_block_size & VM_BLKSZMASK; //free size
    // second_block->size_status |= VM_PREVBUSY; //mark previous block as busy
    // second_block->size_status &= ~VM_BUSY; //mark second block as free

    // //set second block footer
    // struct block_footer* second_footer = (struct block_footer *)((char *)second_block + second_block_size - sizeof(struct block_footer));
    // second_footer->size = second_block_size;

    // //update the first block
    // current_pointer->size_status = rounded_size |
    //                                (current_pointer->size_status & VM_PREVBUSY) |
    //                                VM_BUSY;
}


void *vmalloc(size_t size)
{
    if(size < 1) { 
        return NULL; 
    }

    /**
     * We need to implement the best-fit policy, which is to use the smallest block large enough to hold our data.
     * If there are multiple candidates, use the first available one.
     * 
     * Plan:
     * 
     * Go through the entire heap, mark the pointer to the best possible block
     *      |
     *      |--> Use a while loop for this, and incrememnt the blocks one at a time
     *           If the best possible block is too big, perform a bloack split --> make a helper for tihs
     *              |
     *              |--> If the best block is a 64 byte block, and we need a 16 byte block for vmalloc(4), split
     *                   the 64 byte block into a 16-byte and a 48-byte block, where the 48 byte block will remain free
     */
    int round_up_size = _get_rounded(size);
    struct block_header* current_pointer = heapstart;
    struct block_header* best_fit_block = NULL;
    int current_block_size;

    //getting the pointer to the best fit block
    while(current_pointer->size_status != VM_ENDMARK){
        current_block_size = _get_size_in_bytes(current_pointer);
 
        if (!(current_pointer->size_status & VM_BUSY) && 
            current_block_size >= round_up_size &&
            (best_fit_block == NULL || current_block_size < _get_size_in_bytes(best_fit_block)))
        {
            best_fit_block = current_pointer;
        }

        current_pointer = _get_next_block(current_pointer);
    }

    //if best fit is heapstart, and heapstart is not big enough, return null
    if(best_fit_block == heapstart && _get_size_in_bytes(heapstart) < round_up_size){
        return NULL;
    }
    
    //perform a split
    if(_get_size_in_bytes(best_fit_block) > round_up_size){
        _perform_split(best_fit_block, size);
    }

    // assert((uintptr_t)((char *)best_fit_block + sizeof(struct block_header)) % BLKALIGN == 0);
    // printf("hello");

    return best_fit_block;
}
