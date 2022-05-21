#!/bin/bash
filename=src/Neptune.AppDir/usr/bin/appimageupdatetool-x86_64.AppImage
if [ -f "$filename" ];
then
    :
else
    wget https://github.com/AppImage/AppImageUpdate/releases/download/2.0.0-alpha-1-20220124/appimageupdatetool-x86_64.AppImage -P src/Neptune.AppDir/usr/bin
    chmod +x src/Neptune.AppDir/usr/bin/appimageupdatetool-x86_64.AppImage
fi
filename=src/Neptune.AppDir/usr/bin/aisap-0.6.6-alpha-x86_64.AppImage
if [ -f "$filename" ];
then
    :
else
    wget https://github.com/mgord9518/aisap/releases/download/v0.6.6-alpha/aisap-0.6.6-alpha-x86_64.AppImage -P src/Neptune.AppDir/usr/bin
    chmod +x src/Neptune.AppDir/usr/bin/aisap-0.6.6-alpha-x86_64.AppImage
fi
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun -Wno-format-truncation
gcc src/main.c -g -lappimage -o src/Neptune.AppDir/usr/bin/Neptune
gcc src/installer.c -o src/Neptune.AppDir/usr/bin/installer
gcc src/libregister.c -lappimage -o src/Neptune.AppDir/usr/bin/integrator
appimagetool src/Neptune.AppDir/ > build.output
