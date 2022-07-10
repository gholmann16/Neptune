#!/bin/bash
cp /usr/local/lib/libneptune.so src/Neptune.AppDir/usr/lib/libneptune.so
cp /usr/lib/libiup.so src/Neptune.AppDir/usr/lib/libiup.so
cp /usr/local/lib/libappimage.so.1.0.3 src/Neptune.AppDir/usr/lib/libappimage.so.1.0
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun -Wno-format-truncation
gcc src/main.c -lappimage -liup -lneptune -o src/Neptune.AppDir/usr/bin/Neptune
gcc src/installer.c -o src/Neptune.AppDir/usr/bin/installer
appimagetool src/Neptune.AppDir/ > build.output
