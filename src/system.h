#include <pwd.h>
#include <ctype.h>

int system_wide_registration(char * file, char * name) {
    struct passwd *pwd;
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/integrator", 0), "/tmp/integrator", NULL);

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 999) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /tmp/integrator -i '%s' '%s'", pwd->pw_name, file, name);
            system(cmd);
        }
	}

    remove("/tmp/integrator");
    return 0;
}

int system_wide_unregistration(char * file, char * name) {
    struct passwd *pwd;
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/integrator", 0), "/tmp/integrator", NULL);

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 999) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /tmp/integrator -u '%s' '%s'", pwd->pw_name, file, name);
            system(cmd);
        }
    }

    remove("/tmp/integrator");
    return 0;
}