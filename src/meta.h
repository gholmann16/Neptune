int nepinstall() {
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/installer", 0), "/tmp/installer", NULL);
    int ret = sexecl("/usr/bin/pkexec", "/tmp/installer", getenv("APPIMAGE"), "--install");
    remove("/tmp/installer");
    return ret;
}

int nepuninstall() {
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/installer", 0), "/tmp/installer", NULL);
    int ret = sexecl("/usr/bin/pkexec", "/tmp/installer", getenv("APPIMAGE"), "--uninstall");
    remove("/tmp/installer");
    return ret;
}