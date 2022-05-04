int check_if_installed() {
    if(!access("/etc/Neptune/dir", F_OK ))
        return 0;
    else
        return 1;
}

int checkroot() {
    if(geteuid() != 0) {
        printf("You need to be root to use this program, as it will edit system wide programs.\n");
        exit(1); //return 1
    }
    else if (check_if_installed()) {
        printf("You have not yet installed Neptune or your /etc/Neptune/data file is missing.\n");
        exit(1);
    }
    else 
        return 0;
}