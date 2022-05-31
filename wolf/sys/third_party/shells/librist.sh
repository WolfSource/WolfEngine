#!/bin/bash

if ! [ -d "/librist" ]
then
    echo fetching rist
    git clone --recursive https://code.videolan.org/rist/librist.git
fi
cd librist
meson setup build --backend vs2022 --default-library static --buildtype $1
cd build
ninja