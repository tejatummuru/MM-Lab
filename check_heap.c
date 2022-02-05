
#include "umalloc.h"

//Place any variables needed here from umalloc.c as an extern.
extern memory_block_t *free_head;
int totalf = 0;
extern int ftotal;
       
/* person
 * check_heap -  used to check that the heap is still in a consistent state.
 * Required to be completed for checkpoint 1.
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Example heap check:
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1. (Q1)
        memory_block_t *cur = free_head;
        //goes until it reached the end of the list
        while (cur->next != NULL) {
            //if a block is allocated or not free on the bfree list, immedietly no
            if (is_allocated(cur)) {
                return -1;
            }
            //progresses the pointer
            cur = cur->next;
        }
    //Checking that all the boxes that each item in the list points to is also a free block
    //if a block is pointing to a block that is not free, -1 is returned (Q5)
        //goes until the next has no next to check
        while (cur->next != NULL){
            //if the nect is allocated, immedietly no
            if(is_allocated(cur->next)){
                return -1;
            }
            //else keeps progressing the pointer
            cur = cur->next;
        }

    //checking to see if every free block is on the free list
        //goes through every free block on the free list and adds the total space that is on the list (Q2)
        while (cur->next != NULL){
            totalf+=get_size(cur);
            cur = cur->next;
        }
        //compares the global variable that keeps track of the total amount of free space and sees if there is
        //the same amount of free space on the list and if there is and if not, returns -1

        if(ftotal != totalf){
            return -1;
        } 
    //checking to see if the free list is in memory order (should i add and see if its still the same)
        //goes through every single block on the free list until it reaches the end (Q10)
        while (cur->next != NULL){
            //if the size of the previous is bigger than the next, then the list is not being ordered in 
            //ascending order, so it it not inserting them in the right order and immedietly returns -1
            if(get_size(cur) > get_size(cur->next)){
                return -1;
            }
        }  
    //checking to see if the blocks are aligned correctly (Q3)
        //goes through every single block on the free list until it reaches the end 
        while (cur->next != NULL) {
            //if a block is aligned to the 16 bit size it returns -1 if not
            if(get_size(cur) % 16 != 0){
                return-1;
            }
            //progresses the pointer
            cur = cur->next;
        }
        
    
        
    return 0;
}