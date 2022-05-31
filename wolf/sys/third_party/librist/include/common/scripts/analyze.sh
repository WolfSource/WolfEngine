#!/bin/bash
#
# This script must be run through meson
#
# Checks for the presence of clang static analyzer and/or cppcheck and runs them if present.
#

set -e +x

if [ -z ${MESON_SOURCE_ROOT+x} ]; then
    echo "Missing MESON_SOURCE_ROOT";
    exit 1;
fi

perl -pe "s{\@SOURCEDIR\@}{${MESON_SOURCE_ROOT}}g" ${MESON_SOURCE_ROOT}/common/configs/cppcheck-suppressions.in.txt > ${MESON_SOURCE_ROOT}/common/configs/cppcheck-suppressions.txt

if hash scan-build 2>/dev/null; then
    ninja -C ${MESON_BUILD_ROOT} scan-build
else
    echo "scan-build not found, not running."
fi

if hash cppcheck 2>/dev/null; then
    ninja -C ${MESON_BUILD_ROOT} cppcheck
else
    echo "cppcheck not found, not running."
fi
