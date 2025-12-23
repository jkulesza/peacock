#!/bin/bash

args=("$@")

f=${args[0]}

if [ $# -eq 0 ]
then
    echo Usage: ./montage_python.sh [filename] [ext]
    echo Example: ./montage_python.sh myfile jpg
    echo If no ext is provided, png is assumed
    exit 1
fi

if [ $# -eq 1 ]
then
    echo No file format supplied, assuming png
    ext=png
else
    ext=${args[1]}
fi

echo Converting ${f}.png to colorblind versions...
python3 peacock.py ${f}.png

echo Generating ${f} montage...

montage -label Normal                    ${f}_Normal.${ext} \
        -label Monochromacy              ${f}_Monochromacy.${ext} \
        -label Protanopia                ${f}_Protanopia.${ext} \
        -label Protanomaly               ${f}_Protanomaly.${ext} \
        -label Deuteranopia              ${f}_Deuteranopia.${ext} \
        -label Deuteranomaly             ${f}_Deuteranomaly.${ext} \
        -label Tritanopia                ${f}_Tritanopia.${ext} \
        -label Tritanomaly               ${f}_Tritanomaly.${ext} \
        -geometry 275x275^+1+1 -tile 2x4 ${f}_Montage.${ext}
