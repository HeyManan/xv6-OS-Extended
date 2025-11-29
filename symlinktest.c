#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(void)
{
    int fd;
    char buf[32];

    printf(1, "--- Symlink Test ---\n");

    printf(1, "Creating 'real_file'...\n");
    fd = open("real_file", O_CREATE | O_RDWR);
    write(fd, "SECRET_DATA", 11);
    close(fd);

    printf(1, "Creating symlink 'link_a' -> 'real_file'...\n");
    if(symlink("real_file", "link_a") < 0){
        printf(1, "Symlink creation failed\n");
        exit();
    }

    printf(1, "Reading 'link_a'...\n");
    fd = open("link_a", O_RDONLY);
    if(fd < 0){
        printf(1, "Failed to open link_a\n");
        exit();
    }
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf(1, "Content: %s (Expected: SECRET_DATA)\n", buf);
    close(fd);


    printf(1, "Creating chain 'link_b' -> 'link_a'...\n");
    symlink("link_a", "link_b");
    fd = open("link_b", O_RDONLY);
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf(1, "Content: %s (Expected: SECRET_DATA)\n", buf);
    close(fd);

    printf(1, "Testing O_NOFOLLOW on 'link_a'...\n");
    fd = open("link_a", O_RDONLY | O_NOFOLLOW);

    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf(1, "Content: %s (Expected: real_file)\n", buf);
    close(fd);

    printf(1, "Testing Cycle (A->B, B->A)...\n");
    symlink("cycle_b", "cycle_a");
    symlink("cycle_a", "cycle_b");
    fd = open("cycle_a", O_RDONLY);
    if(fd < 0){
        printf(1, "Cycle detection passed (Open failed as expected)\n");
    } else {
        printf(1, "Cycle detection FAILED (Open succeeded)\n");
        close(fd);
    }

    exit();
}