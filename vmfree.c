#include "vm.h"
#include "vmlib.h"
#include <stdio.h>

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
#include "vm.h"
#include "vmlib.h"
#include <stdio.h>

void vmfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    //convert ptr to block_header
    struct block_header *current_pointer = (struct block_header *)((char *)ptr - sizeof(struct block_header));

    //set the busy bit to 0 (mark the block as free)
    current_pointer->size_status &= ~VM_BUSY;

    //get the current block size
    int current_block_size = current_pointer->size_status & VM_BLKSZMASK;

    //check if the previous block is free (VM_PREVBUSY == 0)
    if ((current_pointer->size_status & VM_PREVBUSY) == 0) {
        //go to the footer of the previous block
        struct block_footer *prev_footer = (struct block_footer *)((char *)current_pointer - sizeof(struct block_footer));

        //get the size of the previous block
        int prev_block_size = prev_footer->size;

        //go to the header of the previous block
        struct block_header *prev_block = (struct block_header *)((char *)prev_footer - (prev_block_size - sizeof(struct block_footer)));

        //update the header of the coalesced block
        int new_block_size = prev_block_size + current_block_size;
        prev_block->size_status = (prev_block->size_status & VM_PREVBUSY) | new_block_size;

        //add a footer to the coalesced block
        struct block_footer *new_footer = (struct block_footer *)((char *)prev_block + new_block_size - sizeof(struct block_footer));
        new_footer->size = new_block_size;

        //update the next block's VM_PREVBUSY bit
        struct block_header *next_block = (struct block_header *)((char *)prev_block + new_block_size);
        if (next_block->size_status != VM_ENDMARK) {
            next_block->size_status &= ~VM_PREVBUSY; // Clear the VM_PREVBUSY bit
        }

        //update the current pointer for further coalescing with the next block
        current_pointer = prev_block;
    }

    //check if the next block is free and can be coalesced
    struct block_header *next_block = (struct block_header *)((char *)current_pointer + BLKSZ(current_pointer));
    if (next_block->size_status != VM_ENDMARK && !(next_block->size_status & VM_BUSY)) {
        int next_block_size = BLKSZ(next_block);

        //update the header of the coalesced block
        int new_block_size = BLKSZ(current_pointer) + next_block_size;
        current_pointer->size_status = (current_pointer->size_status & VM_PREVBUSY) | new_block_size;

        //add a footer to the coalesced block
        struct block_footer *new_footer = (struct block_footer *)((char *)current_pointer + new_block_size - sizeof(struct block_footer));
        new_footer->size = new_block_size;

        //update the VM_PREVBUSY bit for the block after the coalesced block
        struct block_header *next_next_block = (struct block_header *)((char *)current_pointer + new_block_size);
        if (next_next_block->size_status != VM_ENDMARK) {
            next_next_block->size_status &= ~VM_PREVBUSY; // Clear the VM_PREVBUSY bit
        }
    } else {
        //if the next block is busy, add a footer to the current block
        struct block_footer *footer = (struct block_footer *)((char *)current_pointer + BLKSZ(current_pointer) - sizeof(struct block_footer));
        footer->size = BLKSZ(current_pointer);

        //update the VM_PREVBUSY bit for the next block
        if (next_block->size_status != VM_ENDMARK) {
            next_block->size_status &= ~VM_PREVBUSY; // Clear the VM_PREVBUSY bit
        }
    }
}
