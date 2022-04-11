General Purpose:

Neptune is an App Manager for Linux. Its purpose as of now is to integrate installed AppImages into your system.

Features:

    Installs AppImages
    Removes AppImages
    Maintains a list of current AppImages
    Integrates desktop file into the system.

How to use:

1. Run these commands to create the AppImage
    gcc main.c -o Neptune.AppDir/AppRun
    appimagetool-x86_64.AppImage Neptune.AppDir/
2. 
    Install program using the AppImage (sudo ./Neptune-x86_64.AppImage install Neptune-x86_64.AppImage)
3. 
    Command 'nep' will now allow you to add or remove AppImages from your system

Q: Why does Neptune use AppImages?

A: AppImages work on any distribution meaning that they are easier for developers to develop for. They also come bundled with all system resources and are a single executable. In comparison to flatpaks or snaps, they also take up much less storage. And most importantly, over 1000 apps already exist in this format so it's a good choice for an app platform.

Q: Why not manually install?

A: Neptune makes installation more convenient, and integrates the apps into the system.
