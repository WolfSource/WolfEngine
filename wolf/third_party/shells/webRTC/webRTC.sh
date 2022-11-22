#!/bin/bash

# first setup depth_tools https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up

git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=$PWD/depot_tools:$PATH

# now get webrtc-checkout
mkdir webrtc-checkout
cd webrtc-checkout
fetch --nohooks webrtc
gclient sync
cd src

build_type=debug
is_debug = true
if [ $2 = "release" ]; then
    build_type=release
    is_debug = false
fi

if [ $1 = "ios" ]; then
    # Compile version without certificate
    gn gen out/ios_$build_type --args='target_os="ios" target_cpu="arm64" is_debug=$is_debug use_rtti=true is_component_build=false ios_enable_code_signing=false proprietary_codecs=false rtc_include_tests=false' --ide=xcode
    ninja -C out/ios_$build_type

    # Compile version with certificate

    # Get the certificate name
    # security find-identity -v -p codesigning

    # # Compile with the above key
    # gn gen out/ios_$build_type_sign --args='target_os="ios" target_cpu="arm64" is_debug=$is_debug use_rtti=true is_component_build=false ios_code_signing_identity="The string of numbers obtained by the above command" proprietary_codecs=false rtc_use_h264= false rtc_include_tests=false' --ide=xcode
    # ninja -C out/ios_$build_type_sign
else
    # mac & linux
    gn gen out/$build_type --args='target_os="$1" target_cpu="x64" is_debug=$is_debug use_rtti=true is_component_build=false rtc_include_tests=false use_custom_libcxx=false use_custom_libcxx_for_host=false' --ide=xcode
    ninja -C out/$build_type
fi


