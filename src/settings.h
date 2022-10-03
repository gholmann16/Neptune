int defineDir(char *file, char *defo, char *message) {

    FILE *dirFile = fopen(file, "w");
    char answer[MAX_DIR_LEN];
    printf("Where would you like to store ~/%s Default is ~/%s\n~/", message, defo);
    fgets(answer, sizeof answer, stdin);

    if (answer[0] == '\n') {
        printf("Storing to ~/%s directory.\n", defo);
        mkdir(defo, PERMISSIONS);
        fputs(defo, dirFile);
    }
    else {
        answer[strcspn(answer, "\n")] = 0;
        printf("Valid directory dected, saving prefrences.\n");
        mkdir(answer, PERMISSIONS);
        fputs(answer, dirFile);
    }

    fclose(dirFile);
    return 0;
}
