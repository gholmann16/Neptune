#include <pwd.h>

int system_wide_registration(char * file) {
    struct passwd *pwd;
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/integrator", 0), "/tmp/integrator", NULL);

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 127 || pwd->pw_uid == 0) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /tmp/integrator -i '%s'", pwd->pw_name, file);
            system(cmd);
        }
	}

    remove("/tmp/integrator");
    return 0;
}

int system_wide_unregistration(char * file) {
    struct passwd *pwd;
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/integrator", 0), "/tmp/integrator", NULL);

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 127 || pwd->pw_uid == 0) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /tmp/integrator -u '%s'", pwd->pw_name, file);
            system(cmd);
        }
    }

    remove("/tmp/integrator");
    return 0;
}