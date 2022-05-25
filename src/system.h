#include <pwd.h>

int system_wide_registration(char * file, char * name) {
    struct passwd *pwd;

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 999) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /etc/neptune/bin/nep -i '%s' '%s'", pwd->pw_name, file, name);
            system(cmd);
        }
	}

    return 0;
}

int system_wide_unregistration(char * file, char * name) {
    struct passwd *pwd;

    while ((pwd = getpwent()) != NULL) {
        if(pwd->pw_uid > 999) {
            char cmd[512];
            sprintf(cmd, "runuser -u %s -- /etc/neptune/bin/nep -u '%s' '%s'", pwd->pw_name, file, name);
            system(cmd);
        }
    }

    return 0;
}