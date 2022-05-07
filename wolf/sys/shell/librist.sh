#!/bin/bash

if ! [ -d "/librist" ]
then
    echo fetching rist
    git clone --recursive https://code.videolan.org/rist/librist.git
fi
cd librist
if ! [ -d "/build" ]
then
    mkdir build
fi
cd build
meson .. --default-library=static $1
ninja