#!/bin/bash
gcc src/main.c \
    $(pkg-config --libs --cflags gtk+-3.0) \
    lib/neptune.o lib/update.o lib/ezxml.o -lstdc++ \
    lib/libappimage_static.a lib/libboost_filesystem.a lib/libappimage_hashlib.a \
    lib/libaisap-x86_64.a \
    -lappimageupdate \
    -lgcrypt -lcurl -lgpgme \
    lib/libXdgUtilsBaseDir.a lib/libXdgUtilsDesktopEntry.a lib/libxdg-basedir.a \
    -rdynamic -g \
    -lsquashfuse -larchive -lrsvg-2 \
    -o nep
#    lib/libappimageupdate.a lib/libzsync2.a lib/libzsync2_libz.a lib/libcpr.a lib/librcksum.a lib/libupdateinformation.a lib/libutil.a lib/libsigning.a lib/libappimage_shared.a \

