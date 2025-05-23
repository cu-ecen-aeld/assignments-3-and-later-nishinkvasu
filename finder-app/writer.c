// #include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>


int main(int argc, char**argv){
    int fd;
    ssize_t nr;

    openlog(NULL, 0, LOG_USER);
    // setlogmask(LOG_MASK(LOG_EMERG)|LOG_MASK(LOG_ALERT)|LOG_MASK(LOG_CRIT)|LOG_MASK(LOG_ERR)|LOG_MASK(LOG_WARNING));
    setlogmask(LOG_UPTO(LOG_INFO));
    // syslog(LOG_INFO, "Info: Testing syslog - round2!!");

    if(argc < 2){
        // printf("Err: Less than two arguments are passed!\n");
        syslog(LOG_ERR, "Err: Less than two arguments are passed!");
        return 1;
    }
    fd = open(argv[1], O_RDWR | O_CREAT, 0644);
    if (fd < 0){
        // printf("Err: File could not be opened!\n");
        syslog(LOG_ERR, "Err: File \"%s\" could not be opened!!", argv[1]);
        return 1;
    }

    nr = write(fd, (const void *)argv[2], strlen(argv[2]));
    if (nr < 0){
        // printf("Err: Write Error! \n");
        syslog(LOG_ERR, "Err: Write Error! ");
        return 1;
    }
    else if(nr != strlen(argv[2])){
        // printf("Err: Only Partial Write done - %d bytes", nr);
        syslog(LOG_ERR, "Err: Only Partial Write done - %ld bytes", nr);
        return 1;
    }

    return 0;
}