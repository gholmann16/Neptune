#!/bin/bash
if [ "$EUID" -ne 0 ]
  then echo "Neptune must be installed as root."
  exit
fi
mkdir /app
chmod 755 Neptune-x86_64.AppImage
chown root:root Neptune-x86_64.AppImage
mv Neptune-x86_64.AppImage /app
ln -s /app/Neptune-x86_64.AppImage /usr/local/bin/nep