General Purpose:

Neptune is an App Manager for Linux. Its purpose as of now is to integrate installed AppImages into your system. It runs all AppImages inside of a sandbox so you can download appimages from the internet with less stress.

Features:

    Installs Local and Online AppImages
    Maintains a list of AppImage download links
    Removes AppImages
    Can update AppImages
    Integrates and De-Integrates desktop file into the system.
    Runs Appimages in a sandbox 

How to build:

1. 
    Run compile.sh to build Neptune
2. 
    To Install Neptune run: `mv Neptune-x86_64.AppImage /usr/bin/nep`
3. 
    Command 'nep' will now allow you to add or remove AppImages from your system. Can install local AppImages or from appimage.github.io
4. 
    To delete Neptune userdata use: `nep --uninstall` Will delete data for your user only.

Notes: 

Relies on a glibc version more recent than Ubuntu 14.04 (unless you self compile of course).

Q: Why does Neptune use AppImages?

A: AppImages work on any distribution meaning that they are easier for developers to develop for. They also come bundled with all system resources and are a single executable which means they can't affect system dependencies. In comparison to flatpaks or snaps, they also take up much less storage. 

Q: Why not manually install AppImages, as they're supposed to be simple?

A: Neptune makes installation more convenient, and integrates the apps into the system. It can also gets delete userdata when you want to delete the app and can update the AppImages.
