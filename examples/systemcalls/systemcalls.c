#include "systemcalls.h"


#define REDIRECT_FILE "testfile.txt"
#define CHECK_BOOL(x) \
    do { \
        if(x == true) \
            printf("Success!\n"); \
        else \
            printf("Failure!\n"); \
    } while (0)
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret;

    ret = system(cmd);

    if(ret == 0)
        return true;
    else
        return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    pid_t pid;
    int status;
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        printf("command = %s\n", command[i]);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    // In the child, a successful invocation of fork( ) returns 0. In the parent,
    // fork( ) returns the pid of the child.

    pid = fork();
    if (pid == -1)
        return -1;
    else if (pid == 0)
    {
        execv(command[0], command);
        printf("exec failed %d\n", errno);
        exit(-1);

    }

    va_end(args);

    // Below check will be valid only on the parent; 
    // it will wait on the child
    if (waitpid(pid, &status, 0) == -1)
    {
        printf("exec failed %d\n", errno);
        return false;
    }
    else if (WIFEXITED(status)){
        printf("Normal termination with exitstatus %d\n",WEXITSTATUS(status));
        return WEXITSTATUS(status)?false:true;
    }
    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    pid_t pid;
    int status;    
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int fd = open(outputfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0)
    {
        perror("open");
        abort();
    }
    pid = fork();
    if (pid == -1)
        return -1;
    else if (pid == 0)
    {
        // int dup2(int fd1, int fd2);
        // Returns a file descriptor with the value fd2. fd2 now refers to the same file as fd1, 
        // and the file that was previously referred to by fd2 is closed.
        // fd is duplicated to fd==1 which is stdout
        if (dup2(fd, 1) < 0)
        {
            perror("dup2");
            abort();
        }
        close(fd);
        execv(command[0], command);
        printf("exec failed %d\n", errno);
        exit(-1);

    }
    else
        close(fd);

    // int kidpid;
    // int fd = open("redirected.txt", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    // if (fd < 0)
    // {
    //     perror("open");
    //     abort();
    // }
    // switch (kidpid = fork())
    // {
    // case -1:
    //     perror("fork");
    //     abort();
    // case 0:
    //     // int dup2(int fd1, int fd2);
    //     // Returns a file descriptor with the value fd2. fd2 now refers to the same file as fd1, 
    //     // and the file that was previously referred to by fd2 is closed.
    //     // fd is duplicated to fd==1 which is stdout
    //     if (dup2(fd, 1) < 0)
    //     {
    //         perror("dup2");
    //         abort();
    //     }
    //     close(fd);
    //     execv(command[0], command);
    //     perror("execv");
    //     abort();
    // default:
    //     close(fd);
    //     /* do whatever the parent wants to do. */
    // }
    va_end(args);
    // Below check will be valid only on the parent; 
    // it will wait on the child
    if (waitpid(pid, &status, 0) == -1)
    {
        printf("exec failed %d\n", errno);
        return false;
    }
    else if (WIFEXITED(status)){
        printf("Normal termination with exitstatus %d\n",WEXITSTATUS(status));
        return WEXITSTATUS(status)?false:true;
    }
    return false;
}

int main(){
    // do_system(const char *cmd);
    // CHECK_BOOL(do_system("echo this is a test > " REDIRECT_FILE ));
    CHECK_BOOL(do_exec(3,"/bin/sh", "-c", "/usr/bin/ls -la"));
    CHECK_BOOL(do_exec(3, "/usr/bin/test","-f","echo"));
    CHECK_BOOL(do_exec(3, "/usr/bin/test","-f","/bin/echo"));
    CHECK_BOOL(do_exec(3, "/bin/sh", "-c", "echo home is $HOME"));

    do_exec_redirect(REDIRECT_FILE, 3, "/bin/sh", "-c", "echo home is $HOME");
    return 0;
}
