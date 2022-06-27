#include <sys/wait.h>

int sexecl(const char *arg1, const char *arg2, const char *arg3, const char *arg4) {
    pid_t p=fork();
    if(p==0)
    {
        execl(arg1, arg1, arg2, arg3, arg4, NULL );
        perror("execl");
        exit(1);
    }
    waitpid(p, NULL, 0);
}