#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h" // To get priority level constants

void
busy_loop(char *name)
{
    printf(1, "%s: starting busy loop\n", name);
    volatile int i;
    for(i = 0; i < 500000000; i++){}
    printf(1, "%s: finished\n", name);
}

int
main(void)
{
    int pids[3];
    int priorities[3] = {HIGH_PRIORITY, MEDIUM_PRIORITY, LOW_PRIORITY};
    char *names[3] = {"High", "Medium", "Low"};

    printf(1, "--- Priority Scheduler Test ---\n");


    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            set_sched_priority(priorities[i]);

            sleep(100);
            busy_loop(names[i]);
            exit();
        }
    }


    sleep(50);

    printf(1, "Parent: Verifying priorities...\n");
    for (int i = 0; i < 3; i++) {
        int p = get_sched_priority(pids[i]);
        printf(1, "  %s Priority Child (PID %d) has priority %d (Expected %d)\n", names[i], pids[i], p, priorities[i]);
        if(p != priorities[i]){
            printf(1, "  --> ERROR in get_sched_priority!\n");
        }
    }

    printf(1, "Parent waiting for children to finish...\n");

    for (int i = 0; i < 3; i++) {
        wait();
    }

    printf(1, "All children finished.\n");
    exit();
}