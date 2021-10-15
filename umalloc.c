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
    //goes through the list until it reaches the end
    while(cur != NULL && found == false){ 
        //if the size of the block is greater than or equal to the size of the request
        if(get_size(cur) >= size){ 
            found = true;
            return cur;
        }
        //moves pointer
        cur = cur->next;
    }
    return NULL;
}

/*
*finds the previous of a current
* by using the same while loop and 
* having a double trail that will stop when the pointer it is following
* reaching the current block/destination
*/
memory_block_t *findBefore(memory_block_t *block){
    //head and tail
    memory_block_t *cur = free_head;
    memory_block_t *prev = NULL;
    while (cur != NULL){
        //if the head is at the block
        if(cur == block){
            //returns tail
            return prev;
        }
        //tail becomes head, head moves forward
        prev = cur;
        cur = cur->next;
    }
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
    //stores memory in a block
    memory_block_t *more = csbrk(size + sizeof(memory_block_t));
    memory_block_t *ftemp = free_head;
    //if no freehead, this block becomes the freehead
    if(ftemp == NULL){
        ftemp = more;
        return more;
    }
    //goes to the end of the free list
    while(ftemp->next != NULL){
        ftemp = ftemp->next;
    }
    //adds to the end of the list
    ftemp->next = more;
    more->next = NULL;
    return more;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 * by creating a new block that starts at the pointer address of the size and
 * reassigning the pointers
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //changes pointer to the header of the second block
    size_t free_space = get_size(block) - size;
    memory_block_t *temp = (memory_block_t*) ((char*)block + free_space); 
    //updates parameters
    put_block(temp, size, true);
    put_block(block, free_space - sizeof(memory_block_t), false);
    //coalesces to keep space
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
    //stores pointers
    memory_block_t *sbnext = block->next;
    memory_block_t *bfree = sbnext;
    if(block->next != NULL){
        sbnext = bfree->next;
    }
    //gets previous block
    memory_block_t *temp = (memory_block_t*)((char*)bfree - (sizeof(memory_block_t) + get_size(block)));
    if(temp == block && !is_allocated(block->next)){
        //updates the block parameters and pointers
        size_t gmath = get_size(bfree) + get_size(block) + sizeof(memory_block_t);
        put_block(temp, gmath , false);
        temp->next = sbnext;
            return temp;
    }else{
        //updates the pointer
        return block->next;
    }
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 * 
 */
int uinit() {
    //initializes everything
    free_head = csbrk(PAGESIZE * 5);
    ftotal += PAGESIZE * 5;
    size_t gmath = PAGESIZE * 5 - sizeof(memory_block_t);
    put_block(free_head, gmath , false);
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
    //finds the first block
    memory_block_t *cur = find(size);
    //if no block is found, the heap is extended and a new block is added to the list
        if(cur == NULL){
            cur = extend(size); 
            if(cur == NULL){
                return NULL;
            }
        }
    //after the giant extend, the block is split so more free space is available
    if (get_size(cur) > size && (get_size(cur) - (2 * sizeof(memory_block_t)) > size)){ //&&n
        cur = split(cur, size);
        return get_payload(cur);
    }
    //the block is updates
    size_t math = get_size(cur);
    put_block(cur, math, true);    

    //must remove from free list
    memory_block_t *prev = findBefore(cur);
    if(prev != NULL){
        prev->next = cur->next;
    }else{
        //this means that the free head is filled up, so we update the new freehead
        free_head = free_head->next;
        if(free_head == NULL){
            free_head = extend(size);
        }
    }
    //payload is returned
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
    //the pointer is converted to a block and deallocated
    memory_block_t *compare = get_block(ptr);
    memory_block_t *cur = free_head;
    memory_block_t *temp = free_head;
    memory_block_t *prevf = NULL;
    deallocate(compare);
    bool added = false;
    //if no block, ABORT!!!
    if(compare == NULL){
        return;
    }
    //if the address is lower or no free_head, the new free_head is updates
    if(compare < free_head || free_head == NULL){
        memory_block_t *tfree = free_head;
        free_head = compare;
        free_head->next = tfree;
        temp = free_head;
    }else{
        //or else the address's rightful place is found
        while (cur != NULL && added == false){
            //finds the right address
            if(compare < cur){ 
                //pointers are updates to bring in the new block
                memory_block_t *bblock = findBefore(cur);
                //change prev?
                bblock->next = compare;
                compare->next = cur;
                added = true;
                //everything is coalesced after the new block is added
                while (temp->next != NULL){
                    temp = coalesce(temp);
                }
                return;
            }
        //on the occassion we have a null cur, we must now add to the end of the list  
        prevf = cur;
        cur = cur->next; 
    }
    //the case where cur->next is null and we are at the end of the list
    if(cur == NULL){
        if(prevf != NULL){
                //added to the end
                prevf->next = compare;
                compare->next = cur;
            }
        }
    }
    //and everything is coalesced again after adding the new block
    temp = free_head;
    while (temp->next != NULL){
        temp = coalesce(temp);
    }
}