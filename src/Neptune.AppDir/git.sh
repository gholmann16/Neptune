#!/bin/bash
cd /tmp
git clone --depth 1 --filter=blob:none --sparse https://github.com/AppImage/appimage.github.io 
cd appimage.github.io
git sparse-checkout init --cone
git sparse-checkout set data
mv /tmp/appimage.github.io/data /etc/Neptune
rm -rf /tmp/appimage.github.io