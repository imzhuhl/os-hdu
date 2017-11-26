#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    char buf[512] = {0};
    int fd = open("/dev/mycdev", O_RDWR);
    printf("open return: %d\n", fd);
    int ret = read(fd, buf, 20);
    if (ret < 0) {
        printf("read error: %d\n", ret);
        return 0;
    }
    buf[ret] = '\0';
    printf("%s\n", buf);
    lseek(fd, 0, SEEK_SET);

    // write to char_driver
    memset(buf, '\0', sizeof(buf));
    scanf("%s", buf);
    // strcpy(buf, "hello char_driver");
    ret = write(fd, buf, 20);
    if (ret == -1) {
        printf("write error\n");
        return 0;
    }

    lseek(fd, 0, SEEK_SET);
    // read from char_driver
    memset(buf, '\0', sizeof(buf));
    ret = read(fd, buf, 20);
    if (ret < 0) {
        printf("after write, read from char_driver error\n");
    }
    buf[ret] = '\0';
    printf("%s\n", buf);
    close(fd);
    return 0;
}
