#!/bin/bash
cd /tmp
git clone --depth 1 --filter=blob:none --sparse https://github.com/AppImage/appimage.github.io 
cd appimage.github.io
git sparse-checkout init --cone
git sparse-checkout set data
rm -rf /etc/Neptune/data
mv /tmp/appimage.github.io/data /etc/Neptune
cd /etc/Neptune/data
for f in *; do
    test -f "$f" && mv "$f" "${f,,}" &>/dev/null
done
rm -rf /tmp/appimage.github.io