General Purpose:

Neptune is an App Manager for Linux. Its purpose as of now is to integrate installed AppImages into your system.

Features:

    Installs Local and Online AppImages
    Maintains a list of AppImage download links
    Removes AppImages
    Can update AppImages
    Maintains a list of current AppImages
    Integrates and De-Integrates desktop file into the system.

How to build:

1. 
    Run compile.sh to build Neptune
2. 
    To Install Neptune run:
    ```
    sudo mv Neptune.Installer-x86_64.AppImage /usr/local/bin/nep
    sudo nep --install
    ```
3. 
    Command 'nep' will now allow you to add or remove AppImages from your system. Can install local AppImages or from appimage.github.io
4. 
    To uninstall Neptune use `sudo nep --uninstall`

Q: Why does Neptune use AppImages?

A: AppImages work on any distribution meaning that they are easier for developers to develop for. They also come bundled with all system resources and are a single executable. In comparison to flatpaks or snaps, they also take up much less storage. 

Q: Why not manually install AppImages, as they're supposed to be simple?

A: Neptune makes installation more convenient, and integrates the apps into the system. It also gets rid of their files when you want to delete the app and can update the AppImages.
