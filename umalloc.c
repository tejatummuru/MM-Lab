#include "umalloc.h"
#include "csbrk.h"
#include "ansicolors.h"
#include <stdio.h>
#include <assert.h>

/*
* The structure of my free blocks is that they are all connected to the free head and 
* my free list is organized in ascending order in order to make the traversal much easier
* and in order to access memory in a first fit algorithmic system. The structure of my free blocks is 
* not circular because I didn't want to implement a prev pointer, as there would be a lot more to adjust
* for the few occassions i would've needed to access the previous pointer. Whenever the user calls 
* malloc, the methos goes througb the entire free list and finds the first space big enough. if it doesn't 
* find anything, it goes through and coalesces everything together and then calls the find function again to see if 
* any new possible blocks opened up. If nothing is returned again, then there is no avaialable space, so extend is called
* which will open up a big block and since we don't want to give away that much space to a small amount of memory,
* the split function is called to get a more precise amount and the allocated block that has been split will be returned. 
* when the user calls to free a certain block, the pointer of the block is converted to the block we are trying to free, and
* the address of the block is compared to address of the other free blocks in the list and it is added into the right
* chronological place. 
*/

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
void put_blocknn(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc;
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
 *  by using a while loop to find the size of the first block that is big 
 * enough to satisfy its request
 */
memory_block_t *find(size_t size) {
    memory_block_t *cur = free_head;
    bool found = false;
    while(cur != NULL && found == false){ //cur or cur->next?
        if(get_size(cur) >= size){ //not including header, so add seperately?
            found = true;
            return cur;
        }
        cur = cur->next;
    }
    // if(cur!= NULL){
    //     if(get_size(cur) + sizeof(memory_block_t) >= size){
    //         found = true;
    //         return cur;
    //     }
    // }
    return NULL;
}

/*
*finds the previous of a current
* by using the same while loop and 
* having a double trail that will stop when the pointer it is following
* reaching the current block/destination
*/
memory_block_t *findBefore(memory_block_t *block){
    memory_block_t *cur = free_head;
    memory_block_t *prev = NULL;
    //make the freehead based off of this instead, and find way to get size of to the beginning of the pointer, it is only going
    //through things in the free list, so we can't be sure the blocks are all next to each other
    // prev = (memory_block_t*)((char*)cur - (sizeof(memory_block_t)/2)); //minus 1 byte or 8 for pointer? does this go to a block even if it doesn't exist?
    // if(prev == NULL){
    //     return cur;
    // }
    // prev = (memory_block_t*)((char*)prev - get_size(prev) - (sizeof(memory_block_t)/2));
    // return prev;
    // prev = (memory_block_t*)((char*)cur - (sizeof(memory_block_t) + get_size(cur)))
    
    while (cur != NULL){
        if(cur == block){
            return prev;
        }
        prev = cur;
        cur = cur->next;
    }
    
    // if(cur->next == NULL || cur == free_head){
    //     return NULL;
    // }

    // if(cur == free_head || prev == NULL){
    //     return NULL;
    // }
    // while(prev->next != NULL && prev < cur){
    //     if(prev->next >= cur){
    //         return prev;
    //     }
    //     prev = prev->next;
    // }
    // if(prev == cur){
    //     return prev;
    // }
    return NULL;
}


/*
 * extend - extends the heap if more memory is required.
 * by calling cs break and adding more memory and containing
 * it in a block so it can all be accounted for and added to
 * the free list
 */
memory_block_t *extend(size_t size) {
    ftotal+=size;
    memory_block_t *more = csbrk(size + sizeof(memory_block_t));
    memory_block_t *ftemp = free_head;
    memory_block_t *temp = free_head;
    // size_t math = size + sizeof(memory_block_t );
    // put_block(more, math, false);
    if(ftemp == NULL){
        ftemp = more;
        return more;
    }
    while(ftemp->next != NULL){
        ftemp = ftemp->next;
    }
    // more->block_size_alloc = math;
    ftemp->next = more;
    more->next = NULL;
    while (temp->next != NULL){
            temp = coalesce(temp);
        }
    return more;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 * by creating a new block that starts at the pointer address of the size and
 * reassigning the pointers
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    size_t free_space = get_size(block) - size;
    memory_block_t *temp = (memory_block_t*) ((char*)block + free_space); //allocated? i am missing 8 bytes somewhere
    put_block(temp, size, true);
    put_block(block, free_space - sizeof(memory_block_t), false);
    while (temp->next != NULL){
        temp = coalesce(temp);
    }
    return temp;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 * by reassigning the pointers and adjusting the size to be one full block
 */
memory_block_t *coalesce(memory_block_t *block) {
    //make sure the payload is being added correctly and the temp is getting the right values
    //make sure the next and prev are right next to each other by adding the size the header and size of the payload and seeing if it adds to the next
    // memory_block_t *bfree = findBefore(block); 
    // memory_block_t *temp = (memory_block_t*)((char*)bfree + sizeof(memory_block_t) + get_size(block));
    //header 
    memory_block_t *sbnext = block->next;
    //no previous needed
    //only coalesce when free
    // if(temp == block && !is_allocated(bfree)){
    //     bfree = (memory_block_t*)((char*) get_size(bfree) + sizeof(memory_block_t) + get_size(block));
    //     size_t gmath = get_size(bfree) + get_size(block) + sizeof(memory_block_t);
    //     put_block(bfree, gmath , false);
    //     bfree->next = sbnext;
    // }
    memory_block_t *bfree = sbnext;
    if(block->next != NULL){
        sbnext = bfree->next;
    }
    memory_block_t *temp = (memory_block_t*)((char*)bfree - (sizeof(memory_block_t) + get_size(block)));
    if(temp == block && !is_allocated(block->next)){
        // bfree = (memory_block_t*)((char*) block + sizeof(memory_block_t) + get_size(bfree));
        size_t gmath = get_size(bfree) + get_size(block) + sizeof(memory_block_t);
        //+ sizeof(memory_block_t)
        put_block(temp, gmath , false);
        temp->next = sbnext;
            return temp;
    }else{
        return block->next;
    }
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 * 
 */
int uinit() {
    free_head = csbrk(PAGESIZE * 5);
    // printf("%p\n", free_head);
    ftotal += PAGESIZE * 5;
    size_t gmath = PAGESIZE * 5 - sizeof(memory_block_t);
    put_block(free_head, gmath , false);
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
 * find a block that can have as much space as the user requests and puts the memory in the block
 */
void *umalloc(size_t size) {
    size = ALIGN(size);
    memory_block_t *cur = find(size);
        if(cur == NULL){
            cur = extend(size); //do we add this extend into the list?
            if(cur == NULL){
                return NULL;
            }
        }
    if (get_size(cur) > size && (get_size(cur) - (2 * sizeof(memory_block_t)) > size)){ //&&n
        cur = split(cur, size);
        return get_payload(cur);
    }
    size_t math = get_size(cur);
    put_block(cur, math, true);    

    //must remove from free liost
    memory_block_t *prev = findBefore(cur);
    if(prev != NULL){
        prev->next = cur->next;
    }else{
        //this means that the free head is filled up
        free_head = free_head->next;
        if(free_head == NULL){
            free_head = extend(size);
        }
    }
    ftotal-=size;
    return get_payload(cur);
    return NULL;
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 * frees a block by adding it to the free list by adjusting pointers to point to 
 * the new block in the right places
 */
void ufree(void *ptr) {
    memory_block_t *compare = get_block(ptr);
    memory_block_t *cur = free_head;
    memory_block_t *temp = free_head;
    memory_block_t *prevf = NULL;
    bool added = false;
    if(compare == NULL){
        return;
    }
    if(compare < free_head || free_head == NULL){
        memory_block_t *tfree = free_head;
        free_head = compare;
        free_head->next = tfree;
        temp = free_head;
    }else{
        while (cur != NULL && added == false){
            // coalesce(cur);
            if(compare < cur){ //& working?
                memory_block_t *bblock = findBefore(cur);
                //change prev?
                bblock->next = compare;
                compare->next = cur;
                added = true;
                while (temp->next != NULL){
                    temp = coalesce(temp);
                }
                return;
            }
        prevf = cur;
        cur = cur->next; 
    }
    //the case where cur->next is null and we are at the end of the list
    if(cur == NULL){
        if(prevf != NULL){
                prevf->next = compare;
                compare->next = cur;
            }
        }
    }
    temp = free_head;
    while (temp->next != NULL){
        temp = coalesce(temp);
    }
    deallocate(compare);
}