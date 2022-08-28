#!/bin/bash
gcc src/AppRun.c -o src/Neptune.AppDir/AppRun -Wno-format-truncation
gcc src/main.c -I./include lib/libappimage_static.a lib/libboost_filesystem.a -lsquashfuse lib/libXdgUtilsBaseDir.a lib/libXdgUtilsDesktopEntry.a lib/libappimage_hashlib.a lib/update.o lib/libappimageupdate.a lib/libzsync2.a lib/libzsync2_libz.a lib/libcpr.a lib/librcksum.a lib/libupdateinformation.a lib/libutil.a lib/libsigning.a lib/libappimage_shared.a -lcurl -lgpgme -lgcrypt -larchive -lstdc++ -lrsvg-2 lib/ezxml.o lib/libiup.a -lgtk-3 -lgdk-3 -lgdk_pixbuf-2.0 -lpangocairo-1.0 -lpango-1.0 -lcairo   -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lXext -lX11 -lm lib/neptune.o -o src/Neptune.AppDir/usr/bin/Neptune
appimagetool src/Neptune.AppDir/ > build.output
