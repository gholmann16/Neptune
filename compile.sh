#!/bin/bash
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun -Wno-format-truncation
gcc src/main.c \
    $(pkg-config --libs --cflags gtk+-3.0) \
    -rdynamic \
    lib/neptune.o lib/update.o lib/ezxml.o -lstdc++ \
    lib/libappimage_static.a lib/libboost_filesystem.a -lsquashfuse -larchive -lrsvg-2 lib/libXdgUtilsBaseDir.a lib/libXdgUtilsDesktopEntry.a lib/libappimage_hashlib.a \
    -lappimageupdate \
    -o src/Neptune.AppDir/usr/bin/Neptune
#    lib/libappimageupdate.a lib/libzsync2.a lib/libzsync2_libz.a lib/libcpr.a lib/librcksum.a lib/libupdateinformation.a lib/libutil.a lib/libsigning.a lib/libappimage_shared.a -lgcrypt -lcurl -lgpgme \
appimagetool src/Neptune.AppDir/ > build.output
