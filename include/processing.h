#ifndef __PROCESSING__
#define __PROCESSING__

#define RAM_SIZE 4096
#define START_ADDRESS 512

struct cpu
{
    unsigned char* program_counter;
    unsigned char index_register;
    unsigned char v_registers[16];
};


struct memory
{
    unsigned char memory_array[RAM_SIZE];
};


struct processing
{
    struct cpu cpu;
    struct memory memory;
};



struct processing create_memory_and_cpu(void);
struct processing load_program_in_memory(struct processing p, char* pathname);
void dump_memory(struct processing p);

void read_instructions(struct processing p);



#endif // __PROCESSING__
