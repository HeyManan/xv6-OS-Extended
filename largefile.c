#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    char buf[512];
    int fd, i;
    int target_blocks = 16523;

    if(argc > 1)
        target_blocks = atoi(argv[1]);

    printf(1, "--- Large File Test (%d blocks) ---\n", target_blocks);

    fd = open("bigfile", O_CREATE | O_WRONLY);
    if(fd < 0){
        printf(1, "Error: cannot create bigfile\n");
        exit();
    }

    memset(buf, 0, sizeof(buf));

    for(i = 0; i < target_blocks; i++){
        if(i % 100 == 0)
            printf(1, "Writing block %d\n", i);

        if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
            printf(1, "Error: write failed at block %d\n", i);
            exit();
        }
    }

    printf(1, "Success: Wrote %d blocks\n", target_blocks);
    close(fd);
    unlink("bigfile");
    exit();
}