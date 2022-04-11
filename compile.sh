gcc src/main.c -lappimage -o src/Neptune.AppDir/AppRun
gcc installer.c -o Neptune.Installer.AppDir/AppRun
appimagetool src/Neptune.AppDir/
mv Neptune-x86_64.AppImage Neptune.Installer.AppDir/
appimagetool Neptune.Installer.AppDir/