echo "check all args"
gn args out\Debug --list --short
echo "check overrided args"
gn args out\Debug --list --short --overrides-only
echo "set DEPOT_TOOLS_WIN_TOOLCHAIN=0 and set GYP_MSVS_VERSION=2022 to the environment  variables"
echo "build for debug"
gn gen out\Debug --args="target_os=\"win\" target_cpu=\"x64\" is_debug=true use_lld=false is_clang=true use_rtti=true is_component_build=false treat_warnings_as_errors=false rtc_include_tests=false rtc_build_tools=false rtc_build_examples=false enable_libaom=true rtc_enable_protobuf=false rtc_libvpx_build_vp9=true ffmpeg_branding=\"Chrome\" rtc_use_h264=true proprietary_codecs=true use_custom_libcxx=true"
echo "build for release"
gn gen out\Debug --args="is_debug=false"
echo "build"
ninja -C out\Debug
