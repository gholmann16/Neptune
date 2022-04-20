#!/bin/bash
filename=Neptune.Installer.AppDir/appimageupdatetool-x86_64.AppImage
if [ -f "$filename" ];
then
    :
else
    wget https://github.com/AppImage/AppImageUpdate/releases/download/2.0.0-alpha-1-20220124/appimageupdatetool-x86_64.AppImage -P Neptune.Installer.AppDir/
    chmod +x Neptune.Installer.AppDir/appimageupdatetool-x86_64.AppImage
fi
gcc src/main.c -lappimage -o src/Neptune.AppDir/AppRun
gcc installer.c -o Neptune.Installer.AppDir/AppRun
appimagetool src/Neptune.AppDir/ > build.output
mv Neptune-x86_64.AppImage Neptune.Installer.AppDir/
appimagetool Neptune.Installer.AppDir/ >> build.output