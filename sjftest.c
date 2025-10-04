// sjftest.c with sleep to prevent race condition
#include "types.h"
#include "stat.h"
#include "user.h"

void
busy_work(int n)
{
    volatile int i, j;
    for(i = 0; i < n; i++){
        for(j = 0; j < 1000000; j++){
            // do nothing
        }
    }
}

int
main(void)
{
    printf(1, "--- SJF Scheduler Test ---\n");

    // Fork three children
    int pid1 = fork();
    if(pid1 == 0){
        sleep(5); // NEW: Wait a moment before starting work
        printf(1, "Child 1 (short work) starting.\n");
        busy_work(5);
        printf(1, "Child 1 finished.\n");
        exit();
    }

    int pid2 = fork();
    if(pid2 == 0){
        sleep(5); // NEW: Wait a moment before starting work
        printf(1, "Child 2 (long work) starting.\n");
        busy_work(20);
        printf(1, "Child 2 finished.\n");
        exit();
    }

    int pid3 = fork();
    if(pid3 == 0){
        sleep(5); // NEW: Wait a moment before starting work
        printf(1, "Child 3 (medium work) starting.\n");
        busy_work(10);
        printf(1, "Child 3 finished.\n");
        exit();
    }

    printf(1, "Parent: Forked three children.\n");
    // Give a moment for children to be created before checking
    sleep(1);

    printf(1, "Parent checking job lengths:\n");
    printf(1, "  Child 1 (pid %d) predicted length: %d\n", pid1, sjf_job_length(pid1));
    printf(1, "  Child 2 (pid %d) predicted length: %d\n", pid2, sjf_job_length(pid2));
    printf(1, "  Child 3 (pid %d) predicted length: %d\n", pid3, sjf_job_length(pid3));
    printf(1, "Parent waiting for children to finish...\n");

    wait();
    wait();
    wait();

    printf(1, "All children finished.\n");
    exit();
}