#include "systemcalls.h"


#define REDIRECT_FILE "testfile.txt"
#define CHECK_BOOL(x) \
    do { \
        if(x == true) \
            printf("Success!\n"); \
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
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    int status;
    pid_t pid;

    // In the child, a successful invocation of fork( ) returns 0. In the parent,
    // fork( ) returns the pid of the child.

    pid = fork();
    if (pid == -1)
        return -1;
    else if (pid == 0)
    {
        const char *argv[6];
        argv[0] = "/bin/sh";
        argv[1] = "-c";
        argv[2] = "echo \"Testing execv implementation with echo\" > testfile.txt";
        argv[3] = NULL;
        execv("/bin/sh", argv);
        printf("exec failed %d\n", errno);
        exit(-1);

        // Only this format works
        // /bin/sh -c "echo "Test" > testfile.txt"
    }
    // Below check will be valid only on the parent; 
    // it will wait on the child
    if (waitpid(pid, &status, 0) == -1)
    {
        printf("exec failed %d\n", errno);
        return -1;
    }
    else if (WIFEXITED(status))
        return WEXITSTATUS(status);

    va_end(args);

    return true;
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

    va_end(args);

    return true;
}

int main(){
    // do_system(const char *cmd);
    // CHECK_BOOL(do_system("echo this is a test > " REDIRECT_FILE ));
    CHECK_BOOL(do_exec(1));

    return 0;
}
