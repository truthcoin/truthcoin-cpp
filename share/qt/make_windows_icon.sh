#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/truthcoin.png
ICON_DST=../../src/qt/res/icons/truthcoin.ico
convert ${ICON_SRC} -resize 16x16 truthcoin-16.png
convert ${ICON_SRC} -resize 32x32 truthcoin-32.png
convert ${ICON_SRC} -resize 48x48 truthcoin-48.png
convert truthcoin-16.png truthcoin-32.png truthcoin-48.png ${ICON_DST}

