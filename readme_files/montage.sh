#!/bin/bash

args=("$@")

f=${args[0]}

montage -label Normal                    CB_Normal_$f \
        -label Monochromacy              CB_Monochromacy_$f \
        -label Protanopia                CB_Protanopia_$f \
        -label Protanomaly               CB_Protanomaly_$f \
        -label Deuteranopia              CB_Deuteranopia_$f \
        -label Deuteranomaly             CB_Deuteranomaly_$f \
        -label Tritanopia                CB_Tritanopia_$f \
        -label Tritanomaly               CB_Tritanomaly_$f \
        -geometry 275x275^+1+1 -tile 2x4 CB_Montage_$f
