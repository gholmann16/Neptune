#!/bin/bash
cp /usr/local/lib/libneptune.so src/Neptune.AppDir/usr/lib/libneptune.so
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun -Wno-format-truncation
gcc src/main.c -lappimage -liup -lneptune -o src/Neptune.AppDir/usr/bin/Neptune
appimagetool src/Neptune.AppDir/ > build.output
