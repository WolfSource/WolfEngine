#! /bin/bash

display_help() {
    echo "Use:"
    echo "     --bypass_macos           to bypass building APR for MacOS"
    echo "                              type --bypass_macos=1 to disable building"
    echo "                              type --bypass_macos=0 to enable building (default)"
    echo
    echo "     --bypass_ios             to bypass building APR for iPhone"
    echo "                              type --bypass_ios=1 to disable building"
    echo "                              type --bypass_ios=0 to enable building (default)"
    echo
    echo "     --bypass_ios_simulator   to bypass building APR for iPhone-Simulator"
    echo "                              type --bypass_ios_simulator=1 to disable building"
    echo "                              type --bypass_ios_simulator=0 to enable building (default)"
    echo
    echo "     --without-test           to bypass running tests for all platforms"
    echo "                              type --without-test=1 to disable tests"
    exit 1
}

extract_apr_trunk()
{
    ################################
    # Check if apr-trunk.zip is    #
    # available                    #
    ################################
    cd "$DIR/_deps"

    if [ $BYPASS_LINUX == 0 ]
    then
        git clone https://github.com/apache/apr.git -b master
        mv "$DIR/_deps/apr" "$DIR/_deps/apr-src"
    fi
}

#################################### INIT ####################################################
BUILD_DARWIN_VER=`uname -r`
BYPASS_IOS=0
BYPASS_MACOS=0
BYPASS_IOS_SIMULATOR=0
BYPASS_LINUX=0
BYPASS_TESTS=0
BUILD_MODES="Debug"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

################################
# Check if parameters options  #
# are given on the commandline #
################################
while [ "$1" != "" ]; do
    case $1 in
        --bypass_macos=* )          BYPASS_MACOS="${1#*=}"
                                    shift 6
                                    ;;
        --bypass_ios=* )            BYPASS_IOS="${1#*=}"
                                    shift 6
                                    ;;
        --bypass_ios_simulator=* )  BYPASS_IOS_SIMULATOR="${1#*=}"
                                    shift 6
                                    ;;
        --bypass_linux=* )          BYPASS_LINUX="${1#*=}"
                                    shift 6
                                    ;;
        --without-test=* )          BYPASS_TESTS="${1#*=}"
                                    shift 6
                                    ;;
        --Debug )                   BUILD_MODES="Debug"
                                    shift 6
                                    ;;
        --Release )                 BUILD_MODES="Release"
                                    shift 6
                                    ;;
        --build_dir=* )             DIR="${1#*=}"
                                    shift 6
                                    ;;
        -h | --help )               display_help
                                    exit 0
                                    ;;
        * )                         echo "Command not found. Type -h or --help to see available options."
                                    exit
                                    ;;
        esac
        shift
done

if [ $BYPASS_MACOS == 1 ] && [ $BYPASS_IOS == 1 ] && [ $BYPASS_IOS_SIMULATOR == 1 ] && [ $BYPASS_LINUX == 1 ]
then
    echo "No platform remained for building. Returning..."
    exit
fi

if [ $BYPASS_MACOS == 0 ] || [ $BYPASS_IOS == 0 ] || [ $BYPASS_IOS_SIMULATOR == 0 ]
then
    XCODE_ROOT=`xcode-select -print-path`
fi

################################
# Check if expat is available  #
# on building system           #
################################
check_libexpat() {

            if [[ ! -f "$DIR/_deps/libexpat-build/$1/libexpat.a" ]]
            then
                echo "Missing a file! Build libexpat and then try again."
                echo "Building APR failed. Returning..."
            fi
}

################################
# Copy gen_test_char to        #
# /apr-trunk/tools if          #
# --bypass-macos is enabled.   #
# These files are generated    #
# during building apr for MacOS#
# If --bypass-macos is enabled,#
# we copy these pre-built files#
# to /apr-trunk/tools. These   #
# files are needed during      #
# building                     #
################################
if [ $BYPASS_MACOS == 1 && $BYPASS_LINUX == 1 ]
then
    if [[ ! -f "$DIR/tools/gen_test_char" ]] || [[ ! -f "$DIR/tools/gen_test_char.c" ]]
    then
        echo "Missing a file! Extract the apr.zip and run the script again."
        echo "Building APR failed. Returning..."
        exit
    fi
    cp "$DIR/tools/gen_test_char" "$DIR"
    cp "$DIR/tools/gen_test_char.c" "$DIR"
fi

mkdir -p "$DIR/_deps/"
extract_apr_trunk

#BUILD_MODES="Release Debug"

#################################### Building for MacOS ########################################
if [ $BYPASS_MACOS == 0 ]
then

    echo
    echo
    echo "Building APR for MacOS..."
    echo
    echo
    
    ARCHS="x86_64"
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                
                check_libexpat $build_mode $ARCH
                
                cd $DIR/_deps/apr-src
                ./buildconf
                
                PREFIX=$DIR/_deps/apr-build/$build_mode
                CONFIGURE_FLAGS=""
                TARGET=""
                                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O3 -DNDEBUG"
                fi
                
                CONFIGURE_FLAGS="--enable-static --enable-shared"

                if [ "$ARCH" = "x86_64" ]
                then
                    TARGET="x86_64-apple-darwin"
                fi
                
                ./configure \
                    --prefix=$PREFIX \
                    --host=$TARGET \
                    --with-expat=$DIR/_deps/libexpat-build/$1/ \
                    $CONFIGURE_FLAGS \
                    ac_cv_file__dev_zero="yes" \
                    ac_cv_func_setpgrp_void="yes" \
                    apr_cv_process_shared_works="yes" \
                    apr_cv_mutex_robust_shared="no" \
                    apr_cv_tcp_nodelay_with_cork="yes" \
                    ac_cv_sizeof_struct_iovec="8" \
                    apr_cv_mutex_recursive="yes" $@

                if [ "$build_mode" = "Debug" ]
                then
                    make debug=yes
                fi

                if [ "$build_mode" = "Release" ]
                then
                    make debug=no
                fi

                make install
                                
            done
    done
fi

#################################### Building for iOS ########################################
if [ $BYPASS_IOS == 0 ]
then

    echo
    echo
    echo "Building APR for iOS..."
    echo
    echo
    
    DEVROOT="$XCODE_ROOT/Platforms/iPhoneOS.platform/Developer"
    SDKROOT="$DEVROOT/SDKs/iPhoneOS.sdk"

    ARCHS="arm64 armv7 armv7s"
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                                      
                check_libexpat $build_mode $ARCH

                cd $DIR/_deps/apr-src
                ./buildconf
                                
                PREFIX="$DIR/_deps/apr-build/$build_mode/$ARCH"
                
                export PKG_CONFIG_PATH=$XCODE_ROOT/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/lib/pkgconfig:$XCODE_ROOT/Platforms/iPhoneOS.platform/Developer/usr/lib/pkgconfig:/opt/ios/lib/pkgconfig:/usr/local/ios/lib/pkgconfig

                export AS="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/as"
                export ASCPP="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/as"
                export AR="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ar"
                export RANLIB="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib"
                export CPPFLAGS="-arch $ARCH -fembed-bitcode -pipe -I$SDKROOT/usr/include -I$DEVROOT/usr/include -miphoneos-version-min=7.0"
                export CFLAGS="-arch $ARCH -fembed-bitcode -pipe --sysroot='$SDKROOT' -isystem $SDKROOT/usr/include -isystem $DEVROOT/usr/include -miphoneos-version-min=7.0"
                export CXXFLAGS="-arch $ARCH -fembed-bitcode -pipe --sysroot='$SDKROOT' -isystem $SDKROOT/usr/include -isystem $DEVROOT/usr/include -miphoneos-version-min=7.0"
                export LDFLAGS="-arch $ARCH -fembed-bitcode --sysroot='$SDKROOT' -L$SDKROOT/usr/lib -L$DEVROOT/usr/lib -miphoneos-version-min=7.0"
                export CPP=
                export CXXCPP=
                export CC="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
                export CXX="$XCODE_ROOT/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
                export LD="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ld"
                export STRIP="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip"
                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O3 -DNDEBUG"
                fi
                
                CONFIGURE_FLAGS="--enable-static --enable-shared"
                
                TARGET="arm-apple-darwin"

                if [ ! \( -d "$DEVROOT" \) ] ; then
                   echo "The iPhone SDK could not be found. Folder \"$DEVROOT\" does not exist."
                   exit 1
                fi

                if [ ! \( -d "$SDKROOT" \) ] ; then
                   echo "The iPhone SDK could not be found. Folder \"$SDKROOT\" does not exist."
                   exit 1
                fi

                ./configure \
                    --prefix="$PREFIX" \
                    --host=$TARGET \
                    --with-expat=$DIR/_deps/libexpat-build/$1/ \
                    $CONFIGURE_FLAGS \
                    ac_cv_file__dev_zero="yes" \
                    ac_cv_func_setpgrp_void="yes" \
                    apr_cv_process_shared_works="yes" \
                    apr_cv_mutex_robust_shared="no" \
                    apr_cv_tcp_nodelay_with_cork="yes" \
                    ac_cv_sizeof_struct_iovec="8" \
                    apr_cv_mutex_recursive="yes" $@
                    
                    
                # Delete following lines from Makefile, otherwise, the gen_test_char and gen_test_char.c files
                # will be cleaned and not generated during making apr for iOS.
                sed -i -e '/OBJECTS_gen_test_char = tools\/gen_test_char.lo $(LOCAL_LIBS)/d' Makefile
                sed -i -e '/tools\/gen_test_char.lo: tools\/gen_test_char.c/d' Makefile
                sed -i -e '/$(APR_MKDIR) tools/d' Makefile
                sed -i -e '/$(LT_COMPILE)/d' Makefile
                sed -i -e '/tools\/gen_test_char: $(OBJECTS_gen_test_char)/d' Makefile
                sed -i -e '/$(LINK_PROG) $(OBJECTS_gen_test_char) $(ALL_LIBS)/d' Makefile
                
                if [ "$build_mode" = "Debug" ]
                then
                    make debug=yes
                fi

                if [ "$build_mode" = "Release" ]
                then
                    make debug=no
                fi

                make install
            done
    done
fi

#################################### Building for iOS-Simulator ####################################
if [ $BYPASS_IOS_SIMULATOR == 0 ]
then

     echo
     echo
     echo "Building APR for iOS-Simulator..."
     echo
     echo
     
     DEVROOT="$XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer"
     SDKROOT="$DEVROOT/SDKs/iPhoneSimulator.sdk"
     
     ARCHS="i386 x86_64"

     for build_mode in $BUILD_MODES
     do
             for ARCH in $ARCHS
             do
             
                 echo "building $ARCH..."
                                    
                check_libexpat $build_mode $ARCH

                cd $DIR/_deps/apr-src
                ./buildconf
                                
                PREFIX="$DIR/_deps/apr-build/$build_mode/$ARCH"
                
                export PKG_CONFIG_PATH=$XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/usr/lib/pkgconfig:$XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer/usr/lib/pkgconfig:/opt/ios-simulator/lib/pkgconfig:/usr/local/ios-simulator/lib/pkgconfig

                export AS="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/as"
                export ASCPP="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/as"
                export AR="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ar"
                export RANLIB="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib"
                export CPPFLAGS="-arch $ARCH -fembed-bitcode -pipe -I$SDKROOT/usr/include -I$DEVROOT/usr/include -mios-simulator-version-min=7.0"
                export CFLAGS="-arch $ARCH -fembed-bitcode -pipe --sysroot='$SDKROOT' -isystem $SDKROOT/usr/include -isystem $DEVROOT/usr/include -mios-simulator-version-min=7.0"
                export CXXFLAGS="-arch $ARCH -fembed-bitcode -pipe --sysroot='$SDKROOT' -isystem $SDKROOT/usr/include -isystem $DEVROOT/usr/include -mios-simulator-version-min=7.0"
                export LDFLAGS="-arch $ARCH -fembed-bitcode --sysroot='$SDKROOT' -L$SDKROOT/usr/lib -L$DEVROOT/usr/lib -mios-simulator-version-min=7.0"
                export CPP=
                export CXXCPP=
                export CC="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
                export CXX="$XCODE_ROOT/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
                export LD="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/ld"
                export STRIP="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip"
                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O3 -DNDEBUG"
                fi
                
                CONFIGURE_FLAGS="--enable-static --enable-shared"
                
                TARGET="x86_64-apple-darwin11"

                if [ ! \( -d "$DEVROOT" \) ] ; then
                   echo "The iPhone-Simulator SDK could not be found. Folder \"$DEVROOT\" does not exist."
                   exit 1
                fi

                if [ ! \( -d "$SDKROOT" \) ] ; then
                   echo "The iPhone-Simulator SDK could not be found. Folder \"$SDKROOT\" does not exist."
                   exit 1
                fi

                ./configure \
                    --prefix="$PREFIX" \
                    --host=$TARGET \
                    --with-expat=$DIR/_deps/libexpat-build/$1/ \
                    $CONFIGURE_FLAGS \
                    ac_cv_file__dev_zero="yes" \
                    ac_cv_func_setpgrp_void="yes" \
                    apr_cv_process_shared_works="yes" \
                    apr_cv_mutex_robust_shared="no" \
                    apr_cv_tcp_nodelay_with_cork="yes" \
                    ac_cv_sizeof_struct_iovec="8" \
                    apr_cv_mutex_recursive="yes" $@
                    
                    
                # Delete following lines from Makefile, otherwise, the gen_test_char and gen_test_char.c files
                # will be cleaned and not generated during making apr for iOS.
                sed -i -e '/OBJECTS_gen_test_char = tools\/gen_test_char.lo $(LOCAL_LIBS)/d' Makefile
                sed -i -e '/tools\/gen_test_char.lo: tools\/gen_test_char.c/d' Makefile
                sed -i -e '/$(APR_MKDIR) tools/d' Makefile
                sed -i -e '/$(LT_COMPILE)/d' Makefile
                sed -i -e '/tools\/gen_test_char: $(OBJECTS_gen_test_char)/d' Makefile
                sed -i -e '/$(LINK_PROG) $(OBJECTS_gen_test_char) $(ALL_LIBS)/d' Makefile
                
                 if [ "$build_mode" = "Debug" ]
                then
                    make debug=yes
                fi

                if [ "$build_mode" = "Release" ]
                then
                    make debug=no
                fi

                make install
            done
    done
fi
#################################### Building for Linux ########################################
if [ $BYPASS_LINUX == 0 ]
then
    echo
    echo
    echo "Building APR for Linux..."
    echo
    echo
    
    ARCHS="x86_64"
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                
                if [ \( -d "$DIR/linux/$build_mode" \) ]
                then
                    rm -r "$DIR/linux/$build_mode"
                fi
                
                cd $DIR/_deps/apr-src
                ./buildconf
                
                PREFIX="$DIR/_deps/apr-build/$build_mode"
                CONFIGURE_FLAGS=""
                TARGET=""
                                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O3 -DNDEBUG"
                fi
                
                CONFIGURE_FLAGS="--enable-static --enable-shared"
                
                ./configure \
                    --prefix=$PREFIX \
                    $CONFIGURE_FLAGS

                if [ "$build_mode" = "Debug" ]
                then
                    make debug=yes
                fi

                if [ "$build_mode" = "Release" ]
                then
                    make debug=no
                fi

                make install
              
            done
    done
fi
#################################### Finish ####################################
