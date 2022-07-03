int integration() {
    printf("Integrating AppImage: %s", IupGetGlobal("UNFILTERED"));
    install(IupGetGlobal("UNFILTERED"));
    return IUP_DEFAULT;
}

int run_once() {
    printf("Running executable: %s", IupGetGlobal("PROGRAM"));
    sexecl(IupGetGlobal("PROGRAM"), NULL, NULL, NULL);
    return IUP_DEFAULT;
}

int gui(int argc, char **argv, char* appimage)
{

    char* message = "Would you like to integrate this AppImage or Run it once?\n\nIf you integrate the AppImage, it will be installed to your computer like a windows exe. The AppImage will also get a simple sandboxing profile to protect your computer's data.\n\nIf you click run once, the AppImage will run unprotected like a standard linux executable.";

    Ihandle *dlg, *integrate, *run, *text, *hbox, *vbox;

    IupOpen(&argc, &argv);
    
    integrate = IupButton("Integrate", NULL);
    IupSetAttribute(integrate, "SIZE", "60");
    run = IupButton("Run Once", NULL);
    IupSetAttribute(run, "SIZE", "60");

    text = IupLabel(message); //TODO: Languages
    IupSetAttribute(text, "SIZE", "350");
    //IupSetAttribute(text, "FONTSIZE", "16");
    IupSetAttribute(text, "NAME", "TEXT");
    IupSetAttribute(text, "READONLY", "YES");
    IupSetAttribute(text, "WORDWRAP", "YES");
    
    hbox = IupHbox(
        integrate,
        run,
        NULL
    );
    IupSetAttribute(hbox, "GAP", "320");

    vbox = IupVbox(
        text,
        hbox,
        NULL
    );
    IupSetAttribute(vbox, "MARGIN", "15x15");
    IupSetAttribute(vbox, "GAP", "50");

    dlg = IupDialog(vbox);
    IupSetAttribute(dlg, "TITLE", "Neptune AppImage Installer");

    IupSetInt(text, "VALUE", 0);

    IupSetGlobal("UNFILTERED", argv[1]);
    IupSetGlobal("PROGRAM", appimage);

    /* Registers callbacks */
    IupSetCallback(integrate, "ACTION", integration);
    IupSetCallback(run, "ACTION", run_once);

    IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

    IupMainLoop();

    IupClose();
    return EXIT_SUCCESS;
}