#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
int main(){
    int ret, fd;
    fd = open("/dev/grnchar", O_RDONLY);
    if (fd < 0){
        perror("Failed to open the device...");
        return errno;
    }
    int number;
    ret = read(fd, (char*)&number, sizeof(number));
    if (ret < 0){
        perror("Failed to read the number from the device");
        return errno;
    }
    printf("The received number is %d\n", number);
    return 0;
}