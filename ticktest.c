#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
    int my_pid = getpid();
    int start_ticks, end_ticks;

    printf(1, "--- Testing ticks_running() ---\n");


    printf(1, "1. Testing non-existent PID (999): ");
    if(ticks_running(999) == -1)
        printf(1, "OK\n");
    else
        printf(1, "FAIL\n");

    start_ticks = ticks_running(my_pid);
    printf(1, "2. My PID (%d) initial running ticks: %d\n", my_pid, start_ticks);

    printf(1, "3. Performing some work...\n");
    for(int i = 0; i < 200000000; i++){
        // Busy loop
    }

    end_ticks = ticks_running(my_pid);
    printf(1, "4. My PID (%d) final running ticks: %d\n", my_pid, end_ticks);

    if(end_ticks > start_ticks)
        printf(1, "5. Ticks increased: OK\n");
    else
        printf(1, "5. Ticks did not increase: FAIL\n");

    printf(1, "6. Testing unscheduled child process:\n");
    int child_pid = fork();

    if (child_pid < 0) {
        printf(1, "   Fork failed! Test inconclusive.\n");
    } else if (child_pid == 0) {

        exit();
    } else {
        int child_ticks = ticks_running(child_pid);
        printf(1, "   Parent immediately checked child PID %d.\n", child_pid);
        printf(1, "   Ticks returned: %d\n", child_ticks);
        if (child_ticks == 0) {
            printf(1, "   Result: OK\n");
        } else {
            printf(1, "   Result: FAIL (Expected 0)\n");
        }
        wait();
    }

    exit();
}