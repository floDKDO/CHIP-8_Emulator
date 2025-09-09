#ifndef __MEMORY__
#define __MEMORY__

#define RAM_SIZE 4096
#define START_ADDRESS 512

struct memory
{
    unsigned char memory_array[RAM_SIZE];
    unsigned char* program_counter;
};


struct memory create_memory(void);
struct memory load_program_in_memory(struct memory m, char* pathname);
void dump_memory(struct memory m);


#endif // __MEMORY__
