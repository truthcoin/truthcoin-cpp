
Debian
====================
This directory contains files used to package truthcoind/truthcoin-qt
for Debian-based Linux systems. If you compile truthcoind/truthcoin-qt yourself, there are some useful files here.

## truthcoin: URI support ##


truthcoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install truthcoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your truthcoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/truthcoin128.png` to `/usr/share/pixmaps`

truthcoin-qt.protocol (KDE)

