int list() {
    if(!access("/etc/neptune/list", F_OK ))
        return sexecl("/bin/cat", "/etc/neptune/list", NULL, NULL);
    else
        return 6;
}