#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>

#define AESD_PORT 9000
#define DAEMONIZE "-d"
#define BACKLOG 5 // pending connections allowed

const char *fileName = "/var/tmp/aesdsocketdata";
static bool signal_caught = false;

static void signal_handler(int signal_number)
{
    int errno_saved = errno;
    if ((signal_number == SIGINT) || (signal_number == SIGTERM))
        signal_caught = true;
    errno = errno_saved;
}

int main(int argc, char **argv)
{
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
    struct sigaction new_sigaction;
    int opt = 1;
    int newsockfd;
    socklen_t addr_size;
    struct sockaddr_storage clientaddr;

    signal_caught = false;
    // set up signal handler

    memset(&new_sigaction, 0, sizeof(struct sigaction));
    new_sigaction.sa_handler = signal_handler;

    if (sigaction(SIGTERM, &new_sigaction, NULL) != 0)
    {
        printf("Error registering for SIGTERM");
    }
    if (sigaction(SIGINT, &new_sigaction, NULL) != 0)
    {
        printf("Error registering for SIGINT");
    }

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

// daemonize if the option is passed
    printf("argc = %d\n", argc);
    if (argc == 2)
    {
        if (strncmp(argv[1], DAEMONIZE, strlen(DAEMONIZE)) == 0)
        {
            // fork
            // exit parent
            // setsid - create new session to ensure no controlling tty
            // chdir to "/"
            // close fds

            pid_t pid = fork();
            if (pid < 0)
                return -1; // fork failed

            if (pid > 0)
            {
                printf("Exiting parent process - child pid %d \n", pid);
                exit(0);
            }

            if (setsid() < 0)
                exit(1);

            if (chdir("/") < 0)
                exit(1);

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            open("/dev/null", O_RDWR);
            dup(0);
            dup(0);
        }
    }

    if (listen(sockfd, BACKLOG) < 0)
    {
        perror("listen failed");
        printf("listen failed \n");
        syslog(LOG_ERR, "AESD: Listen failed!");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_ERR, "AESD: before open and accept!");

    addr_size = sizeof clientaddr;

    // open file for storing the collected data
    // char* fileName = "aesdsocketdata";
    // while creating daemon the above will fail as we change to the root directory
    int filefd = open(fileName, O_RDWR | O_CREAT, 0644);
    if (filefd < 0)
    {
        // printf("Err: File could not be opened!\n");
        syslog(LOG_ERR, "AESD: File \"%s\" could not be opened!!", fileName);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if (signal_caught == true)
        {
            close(sockfd);
            close(filefd);
            remove(fileName);
            syslog(LOG_INFO, "AESD: Server shutting down gracefully");
            exit(EXIT_SUCCESS);
        }
        printf("before accept \n");
        // accept should also be in a loop and then should handle SIGINT
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addr_size)) < 0)
        {
            if (newsockfd == -1)
            {
                if ((errno == EINTR) && (signal_caught == true))
                {
                    close(sockfd);
                    close(filefd);

                    if (!remove(fileName))
                        printf("File remove failed \n");

                    printf("accept failed due to SIGINT or SIGTERM\n");
                }
            }
            perror("accept failed");
            printf("accept failed \n");
            syslog(LOG_ERR, "AESD: accept failed!!");

            exit(EXIT_FAILURE);
        }

        char buf[512];
        int recv_count;

        // receive data in a loop to ensure big buffers of data can be handled
        ssize_t nr;
        do
        {
            // printf("before recv \n");
            recv_count = recv(newsockfd, buf, sizeof buf, 0);
            if (recv_count > 0)
            {
                char *tmpbuf = (char *)malloc((sizeof buf) + 1);
                memset(tmpbuf, 0, (sizeof buf) + 1);
                strncpy(tmpbuf, (const char *)&buf, recv_count);
                tmpbuf[recv_count] = '\0';
                printf("string received %s\n", tmpbuf);
                free(tmpbuf);

                // write the buffer into the file with proper partial write handling
                size_t bytes_written = 0;
                while (bytes_written < recv_count)
                {
                    nr = write(filefd, (const void *)(buf + bytes_written), recv_count - bytes_written);
                    if (nr < 0)
                    {
                        if (errno == EINTR)
                        {
                            continue; // Interrupted by signal, try again
                        }
                        syslog(LOG_ERR, "AESD: Write Error: %s", strerror(errno));
                        close(filefd); // had to close the file before reading back again
                        if (!remove(fileName))
                            printf("File remove failed \n");
                        close(newsockfd);
                        exit(EXIT_FAILURE);
                    }
                    bytes_written += nr;
                }
                syslog(LOG_DEBUG, "AESD: Successfully wrote %zu bytes to file", bytes_written);
            }
            else if (recv_count == 0)
            {
                syslog(LOG_ERR, "AESD: client disconnected!! ");
                printf("client disconnected!!\n");
                break; // added precaution to break out of infinite loop
                       // although code should not reach here ideally
            }
            else
            {
                syslog(LOG_ERR, "AESD: recv failed!! ");
                perror("recv failed");
                break;
            }
        } while (recv_count > 0 && buf[recv_count-1] != '\n');

        // fsync(filefd); // way to flush to disk
        close(filefd); // had to close the file before reading back again
        filefd = open(fileName, O_RDWR, 0644);
        if (filefd < 0)
        {
            // printf("Err: File could not be opened!\n");
            syslog(LOG_ERR, "AESD: File \"%s\" could not be opened!!", fileName);
            exit(EXIT_FAILURE);
        }
        // Send the file contents back from here
        // Will need to read the file contents till we reach a newline
        printf("reading file!!\n");
        memset(buf, 0, (sizeof buf));
        do
        {
            nr = read(filefd, buf, sizeof(buf));
            if (nr < 0)
            {
                printf("Err: Read Error! \n");
                syslog(LOG_ERR, "AESD: Read Error! ");
                exit(EXIT_FAILURE);
            }

            // Verifying read part below
            char *tmpbuf = (char *)malloc((sizeof buf) + 1);
            memset(tmpbuf, 0, (sizeof buf) + 1);
            strncpy(tmpbuf, (const char *)&buf, nr);
            tmpbuf[nr] = '\0';
            printf("%s", tmpbuf);
            // printf("%d string read %s", (int)nr, tmpbuf);
            free(tmpbuf);

            // send file contents back over the socket
            // ssize_t send(int s, const void *buf, size_t len, int flags);
            if (nr > 0)
                if (send(newsockfd, buf, nr, 0) == -1)
                {
                    syslog(LOG_ERR, "AESD: send Error! ");
                    perror("send");
                }
            // ssize_t sendcount =

        } while (nr > 0);
        close(newsockfd);
    }

    close(sockfd);
    close(filefd);

    return EXIT_FAILURE;
}
