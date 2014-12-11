#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

struct block_meta {
	size_t size;
	struct block_meta *next;
	int free;
	int magic;
	char data[0];
};

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
	struct block_meta *current = global_base;
	while (current && !(current->free && current->size >= size)){
		*last = current;
		current = current->next;		
	}
	return current;
}

struct block_meta *request_space(struct block_meta *last, size_t size)
{
	struct block_meta *block = NULL;
	block = sbrk(0);
	void *request = sbrk(size + sizeof(struct block_meta));
	if (request == (void *)(-1))
		return NULL;
	assert(block == (struct block_meta *)request);

	if (last)
		last->next = block;
	else{
		block->size = size;
		block->next = NULL;
		block->free = 0;
		block->magic = 0x12345678;
	}
	return block;
}

void *my_malloc(size_t size)
{
	struct block_meta *block;

	if (size <= 0)
		return NULL;

	if (!global_base){
		block = request_space(NULL, size);
		if (!block)
			return NULL;
		global_base = block;
	} else {
		struct block_meta *last = global_base;
		block = find_free_block(&last, size);
		if (!block){
			block = request_space(last,size);
			if (!block)
				return NULL;
		} else {
			block->free = 0;
			block->magic = 0x77777777;
		}
	}

	return block->data;
}

struct block_meta *get_block_ptr(void *ptr)
{
	return (struct block_meta *)((char *)ptr-sizeof(struct block_meta));
}

void my_free(void *ptr)
{
	if (!ptr)
		return;

	struct block_meta *block_ptr = get_block_ptr(ptr);
	
	assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
	block_ptr->free = 1;
	block_ptr->magic = 0x55555555;	
}
