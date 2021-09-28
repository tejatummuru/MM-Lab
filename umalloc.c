#include "umalloc.h"
#include "csbrk.h"
#include "ansicolors.h"
#include <stdio.h>
#include <assert.h>

const char author[] = ANSI_BOLD ANSI_COLOR_RED "TEJASVINI TUMMURU TT26586" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;

/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 * todo
 */
memory_block_t *find(size_t size) {
    memory_block_t *cur = free_head;
    bool found = false;
    while(cur->next != NULL && found == false){
        printf("entering the loop");
        if(get_size(cur) >= size){
            found = true;
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 * todo
 */
memory_block_t *extend(size_t size) {
    return csbrk(16 * PAGESIZE);
}

/*
 * split - splits a given block in parts, one allocated, one free.
 * todo
 */
memory_block_t *split(memory_block_t *block, size_t size) {

    return NULL;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 * todo
 */
memory_block_t *coalesce(memory_block_t *block) {
    return NULL;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 * todo
 */
int uinit() {
    free_head = csbrk(PAGESIZE * 12);
    put_block(free_head, PAGESIZE * 12, false);
    // free_head->next = NULL;
    //set size, next, 
   //how do i set the metadata? like allocated = true? where do i do that? also where do i do the if else for this

   if(free_head == NULL){
       return -1;
   }
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 * todo
 */
void *umalloc(size_t size) {

    memory_block_t *cur = find(size);
    if(cur == NULL){
        cur = extend(size);
    }
    put_block(cur, get_size(cur), true);
    allocate(cur);
    return get_payload(cur);

    // memory_block_t *cur = free_head;
    // memory_block_t *prev = free_head;
    // //sort the blocks in ascending order
    // bool found = false;

    // while (found == false){
    //     printf("entering the loop");
    //     if(get_size(cur)- sizeof(memory_block_t) == size){ //or do we do get_size() here?
    //         found = true;
    //         allocate(cur); //take it out of free list
    //         prev->next = cur->next;
    //         // cur->next = cur->next->next; //is this how we remove from the free list?
    //         return get_payload(cur);
    //     }else if(get_size(cur) - sizeof(memory_block_t) > size){ //do split later but for now, just get rid of the block
    //         // memory_block_t *result = cur;
    //         // result = split(cur, size);
    //         found = true;
    //         allocate(cur);
    //         prev->next = cur->next;
    //         // cur->next = cur->next->next; //is this how we remove from the free list?
    //         return get_payload(cur);
    //     }else if(cur->block_size_alloc < size){
    //         if(cur->next == NULL){
    //             free_head = csbrk(size);
    //             found = true;
    //         }
    //         //we want to check the free list
    //         prev = cur;
    //         cur = cur->next; //loop
    //         //and then return a new block  
    //     }
    // }
    
    
    return NULL;
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 * todo
 */
void ufree(void *ptr) {
    // memory_block_t *compare = get_block(ptr);
    // deallocate(ptr);
    // put_block(compare, get_size(compare), is_allocated(compare));
    // //get_block to translate to payload->block
    // //put_block to make free block and then while loop
    // memory_block_t *cur = free_head;
    // //do i just call put instead of a;ll this?
    // bool added = false;
    // while (cur->next != NULL && added == false){
    //     printf("going in and not coming out");
    //     //just compare pointers
    //     if(compare < cur){
    //         compare->next = cur->next;
    //         cur->next = ptr;
    //         added = true;
    //         return;
    //     }
    //     cur = cur->next;
    // }
    // cur->next = ptr;
    //where do we access the list/add to the free list?
}