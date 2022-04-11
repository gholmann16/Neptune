#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int sexecl(char *arg1, char *arg2, char *arg3, char *arg4) {
    pid_t p=fork();
    if(p==0)
    {
        execl(arg1, arg1, arg2, arg3, arg4, NULL );
        perror("execl");
        exit(1);
    }
    waitpid(p, NULL, 0);
}