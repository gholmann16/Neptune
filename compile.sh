#!/bin/bash
filename=src/Neptune.AppDir/usr/bin/appimageupdatetool-x86_64.AppImage
if [ -f "$filename" ];
then
    :
else
    wget https://github.com/AppImage/AppImageUpdate/releases/download/2.0.0-alpha-1-20220124/appimageupdatetool-x86_64.AppImage -P src/Neptune.AppDir/usr/bin
    chmod +x src/Neptune.AppDir/usr/bin/appimageupdatetool-x86_64.AppImage
fi
filename=src/Neptune.AppDir/usr/bin/aisap-0.5.9-alpha-x86_64.AppImage
if [ -f "$filename" ];
then
    :
else
    wget https://github.com/mgord9518/aisap/releases/download/continuous/aisap-0.5.9-alpha-x86_64.AppImage -P src/Neptune.AppDir/usr/bin
    chmod +x src/Neptune.AppDir/usr/bin/aisap-0.5.9-alpha-x86_64.AppImage
fi
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun
gcc src/main.c -lappimage -o src/Neptune.AppDir/usr/bin/Neptune
gcc installer.c -o Neptune.Installer.AppDir/AppRun
appimagetool src/Neptune.AppDir/ > build.output
mv Neptune-x86_64.AppImage Neptune.Installer.AppDir/
appimagetool Neptune.Installer.AppDir/ >> build.output