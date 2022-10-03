int installed() {
    chdir(getenv("HOME"));
    return access(".config/neptune/dir", F_OK);
}