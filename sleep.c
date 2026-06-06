#include "user.h"

int
main(int argc, char *argv[])
{
    int ticks;

    if (argc != 2) {
        printf(2, "Usage: sleep [ticks]\n");
        exit();
    }

    ticks = atoi(argv[1]);
    if (ticks > 0) {
        sleep(ticks);
    }
    exit();
}
