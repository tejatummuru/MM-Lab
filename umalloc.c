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
int ftotal = 0;

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
        if(get_size(cur) >= size){
            found = true;
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

/*
*finds the previous of a current
*/
memory_block_t *findBefore(memory_block_t *block){
    memory_block_t *cur = block;
    memory_block_t *prev = free_head;
    while(prev->next != NULL && prev != cur){
        if(prev->next == cur){
            return prev;
        }
        prev = prev->next;
    }
    if(prev == cur){
        return cur;
    }
    return NULL;
}


/*
 * extend - extends the heap if more memory is required.
 * todo
 */
memory_block_t *extend(size_t size) {
    ftotal+=16*PAGESIZE;
    return csbrk(16 * PAGESIZE);
}

/*
 * split - splits a given block in parts, one allocated, one free.
 * todo
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //putting a block of this size into the list
    //return free
    //create a temporary pointer 
    memory_block_t *temp = block; 
    memory_block_t *bfree = block;
    //get the pointer to a place on the block where we want to split using pointer arithmatic
    //the components of this are the original temp + the header size(because I believe  its not included) + 
    //(the size of the block - the size we want allocated since we do not want to reassign the pointers),, 
    //if there is an error check if we add temp to itself,, do i call malloc on this? you don't because i would call split 
    //in malloc which would lead to a lgic error
    temp = (memory_block_t*) ((char*)temp + (sizeof(memory_block_t) + (get_size(block) - size)));
    //once we have moved the pointer, we want to clarify this as a new block that is taken
    put_block(temp, get_size(temp), true);
    //make sure the other block is free, but do we need to put and return another block for that?
    put_block(bfree, get_size(block) - sizeof(temp), false);
    return temp;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 * todo
 */
memory_block_t *coalesce(memory_block_t *block) {
    //make sure the payload is being added correctly and the temp is getting the right values
    //make sure the next and prev are right next to each other by adding the size the header and size of the payload and seeing if it adds to the next
    memory_block_t *bfree = block; 
    memory_block_t *temp = (memory_block_t*)((char*)bfree + sizeof(memory_block_t) + get_size(block));
    memory_block_t *sbnext = block->next;
    if(temp == block && !is_allocated(bfree)){
        bfree = findBefore(block);
        bfree = (memory_block_t*)((char*) bfree + sizeof(memory_block_t) + get_size(block));
        put_block(bfree, get_size(bfree) + get_size(block) + sizeof(memory_block_t), false);
        bfree->next = sbnext;
    }
    bfree = block->next;
    if(block->next != NULL){
            sbnext = bfree->next;
    }
    //pointers can't move backwards can they? this isn't illegal tho?
    temp = (memory_block_t*)((char*)bfree - (sizeof(memory_block_t) + get_size(block)));
    if(temp == block && !is_allocated(block->next)){
        bfree = (memory_block_t*)((char*) bfree + sizeof(memory_block_t) + get_size(block));
        put_block(bfree, get_size(bfree) + get_size(block) + sizeof(memory_block_t), false);
        if(block->next != NULL){
            bfree->next = sbnext;
        }
    }
    return bfree;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 * todo
 */
int uinit() {
    free_head = csbrk(PAGESIZE * 12);
    ftotal += PAGESIZE * 12;
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
    if (get_size(cur) > size){
        cur = split(cur, size);
    }
    // else if(get_size(cur) < size){
    //     cur = coalesce(cur);
    // }
    put_block(cur, get_size(cur), true);
    allocate(cur);
    //must remove from free liost
    memory_block_t *prev = findBefore(cur);
    if(cur->next != NULL){
        prev->next = cur->next;
    }
    ftotal-=size;
    return get_payload(cur);

    // memory_block_t *cur = free_head;
    // memory_block_t *prev = free_head;
    // //sort the blocks in ascending order
    // bool found = false;

    // while (found == false){
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
    //}
    
    
    return NULL;
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 * todo
 */
void ufree(void *ptr) {
    memory_block_t *compare = get_block(ptr);
    deallocate(compare);
    put_block(compare, get_size(compare), is_allocated(compare));
    //get_block to translate to payload->block
    //put_block to make free block and then while loop
    memory_block_t *cur = free_head;
    //do i just call put instead of a;ll this?
    bool added = false;
    while (cur->next != NULL && added == false){
        //just compare pointers
        if(compare < cur){
            //change prev?
            compare->next = cur;
            added = true;
            return;
        }
        cur = cur->next;
    }
}