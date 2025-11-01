#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#define AESD_PORT 9000

int main(int argc, char **argv)
{
    printf("Hello Server!!\n");

    // int status;
    // struct addrinfo hints;
    // struct addrinfo *servinfo;

    // memset(&hints, 0, sizeof(hints));

    // hints.ai_family = AF_UNSPEC; // IPv4 or v6
    // hints.ai_socktype = SOCK_STREAM; // TCP sockets
    // hints.ai_flags = AI_PASSIVE; //will fill in the IP

    // if((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0){

    //     fprintf(stderr, "gai error = %s\n", gai_strerror(status));
    //     return 0;
    // }

    int sockfd;
    struct sockaddr_in my_addr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    my_addr.sin_family = AF_INET; // IPv4
    my_addr.sin_port = htons(AESD_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY; // bind your local IP address
    memset(my_addr.sin_zero, '\0', sizeof my_addr);

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof opt))
    {
        perror("setsockopt");
        printf("setsockopt failed \n");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) < 0)
    {
        perror("bind failed");
        printf("bind failed \n");
        exit(EXIT_FAILURE);
    }

#define BACKLOG 5 // pending connections allowed
    if (listen(sockfd, BACKLOG) < 0)
    {
        perror("listen failed");
        printf("listen failed \n");
        exit(EXIT_FAILURE);
    }

    int newsockfd;
    socklen_t addr_size;
    struct sockaddr_storage clientaddr;
    addr_size = sizeof clientaddr;

    // open file for storing the collected data

    int filefd = open("aesdsocketdata", O_RDWR | O_CREAT, 0644);
    if (filefd < 0)
    {
        // printf("Err: File could not be opened!\n");
        syslog(LOG_ERR, "Err: File \"%s\" could not be opened!!", argv[1]);
        return 1;
    }
    while (1)
    {
        printf("before accept \n");
        // accept should also be in a loop and then should handle SIGINT
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addr_size)) < 0)
        {
            perror("accept failed");
            printf("accept failed \n");
            exit(EXIT_FAILURE);
        }

        char buf[10];
        int recv_count;

        // receive data in a while loop to ensure big buffers of data can be handled
        // while(1)
        // {
        do
        {
            printf("before recv \n");
            recv_count = recv(newsockfd, buf, sizeof buf, 0);
            if (recv_count > 0)
            {
                char *tmpbuf = (char *)malloc((sizeof buf) + 1);
                memset(tmpbuf, 0, (sizeof buf) + 1);
                strncpy(tmpbuf, (const char *)&buf, recv_count);
                tmpbuf[recv_count + 1] = '\0';
                printf("string received %s\n", tmpbuf);
                free(tmpbuf);

                // write the buffer into the file
                // another option is to store it into a big enough buffer and write it at last
                // will need to see what happens if multiple buffers are written if it introduces a new line
                ssize_t nr = write(filefd, (const void *)buf, recv_count);
                if (nr < 0)
                {
                    // printf("Err: Write Error! \n");
                    syslog(LOG_ERR, "Err: Write Error! ");
                    return 1;
                }
                else if (nr != recv_count)
                {
                    // printf("Err: Only Partial Write done - %d bytes", nr);
                    syslog(LOG_ERR, "Err: Only Partial Write done - %ld bytes", nr);
                    return 1;
                }
            }
            else if (recv_count == 0)
            {
                printf("client disconnected!!\n");
            }
            else
            {
                perror("recv failed");
            }
        } while (recv_count > 0);

        close(newsockfd);
    }

    close(sockfd);
    close(filefd);
}
