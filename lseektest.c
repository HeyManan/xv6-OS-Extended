#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(void)
{
    int fd;
    char buf[10];

    printf(1, "--- lseek Test ---\n");

    fd = open("lseek_test.txt", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(1, "Error creating file\n");
        exit();
    }

    printf(1, "Writing 'Start' at beginning...\n");
    if(write(fd, "Start", 5) != 5){
        printf(1, "Write failed\n");
        exit();
    }

    printf(1, "Seeking forward by 5 bytes...\n");
    if(lseek(fd, 5) < 0){
        printf(1, "lseek failed\n");
        exit();
    }

    printf(1, "Writing 'End'...\n");
    if(write(fd, "End", 3) != 3){
        printf(1, "Write failed\n");
        exit();
    }

    close(fd);

    printf(1, "Verifying file content...\n");
    fd = open("lseek_test.txt", O_RDONLY);

    read(fd, buf, 5);
    buf[5] = 0;
    printf(1, "Bytes 0-4: '%s' (Expected 'Start')\n", buf);

    read(fd, buf, 5);
    int hole_ok = 1;
    for(int i=0; i<5; i++){
        if(buf[i] != 0) hole_ok = 0;
    }
    if(hole_ok)
        printf(1, "Bytes 5-9: [Zero-filled] (Expected)\n");
    else
        printf(1, "Bytes 5-9: [NOT Zero-filled] (ERROR)\n");

    read(fd, buf, 3);
    buf[3] = 0;
    printf(1, "Bytes 10-12: '%s' (Expected 'End')\n", buf);

    close(fd);
    exit();
}