int nepremove(char *name) {
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/local/bin/neproot", 0), "/tmp/neproot", NULL);
    int ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", "-R", name);
    remove("/tmp/neproot");
    return ret; //change these returns to a verbose chosen printf
}