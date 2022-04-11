int checkroot() {
    if(geteuid() != 0) {
        printf("You need to be root to use this program, as it will edit system wide programs.\n");
        exit(1); //return 1
    }
}