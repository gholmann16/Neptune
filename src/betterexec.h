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

 int runAs(int gid, int uid, char **env, char *arg1, char *arg2, char *arg3, char *arg4) {
    int status;
    int child = fork();
    if (child == 0) {
    setgid(100);
        setuid(1000);
        do {
        putenv(*env);
        env++;
        } while (env != NULL);
        sexecl(arg1, arg2, arg3, arg4);
    } else if (child > 0) {
    waitpid(child,&status,WUNTRACED | WCONTINUED);
    } else {
        // Error: fork() failed!
    }
 }