#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

// This program accesses 10 pages of heap memory sequentially.
int
main(void)
{
    int num_pages = 10;

    printf(1, "--- Heap Test: Accessing %d pages ---\n", num_pages);

    char *mem = sbrk(num_pages * PGSIZE);
    if (mem == (char*)-1) {
        printf(2, "heaptest: sbrk failed\n");
        exit();
    }


    for (int i = 0; i < num_pages; i++) {
        printf(1, "Accessing page %d at address 0x%x\n", i, (mem + i*PGSIZE));
        // This is the line that triggers the page fault
        mem[i * PGSIZE] = 'A';
    }

    printf(1, "--- Heap Test: Done ---\n");
    exit();
}