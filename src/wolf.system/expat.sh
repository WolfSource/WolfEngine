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

extract_expat()
{
    ################################
    # Check if apr-trunk.zip is    #
    # available                    #
    ################################
    cd "$DIR/_deps"
    
    git clone https://github.com/libexpat/libexpat.git
    
    mv "$DIR/_deps/libexpat" "$DIR/_deps/libexpat-src"
}

#################################### INIT ####################################################
BUILD_DARWIN_VER=`uname -r`
XCODE_ROOT=`xcode-select -print-path`
BYPASS_IOS=0
BYPASS_MACOS=0
BYPASS_IOS_SIMULATOR=0
BYPASS_TESTS=0
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
IS_EXPAT_LIB=1
IS_EXPAT_INCLUDE=1

################################
# Check if parameters options  #
# are given on the commandline #
################################
while [ "$1" != "" ]; do
    case $1 in
        --bypass_macos=* )          BYPASS_MACOS="${1#*=}"
                                    shift 4
                                    ;;
        --bypass_ios=* )            BYPASS_IOS="${1#*=}"
                                    shift 4
                                    ;;
        --bypass_ios_simulator=* )  BYPASS_IOS_SIMULATOR="${1#*=}"
                                    shift 4
                                    ;;
        --without-test=* )          BYPASS_TESTS="${1#*=}"
                                    shift 4
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

if [ $BYPASS_MACOS == 1 ] && [ $BYPASS_IOS == 1 ] && [ $BYPASS_IOS_SIMULATOR == 1 ]
then
    echo "No platform remained for building. Returning..."
    exit
fi

mkdir -p "$DIR/_deps/"
extract_expat

BUILD_MODES="Release Debug"

#################################### Building for MacOS ########################################
if [ $BYPASS_MACOS == 0 ]
then

    echo
    echo
    echo "Building Expat for MacOS..."
    echo
    echo
    
    ARCHS="x86_64"
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                
                cd $DIR/_deps/libexpat-src/expat
                ./buildconf.sh
                
                PREFIX=$DIR/_deps/libexpat-build/MacOS/$build_mode/$ARCH
                CONFIGURE_FLAGS=""
                TARGET=""
                                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O2 -DNDEBUG"
                fi
                
                if [ "$ARCH" = "x86_64" ]
                then
                    TARGET="x86_64-apple-darwin"
                fi
                
                ./configure \
                    --prefix=$PREFIX \
                    --host=$TARGET \
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

#################################### Building for iOS ########################################
if [ $BYPASS_IOS == 0 ]
then

    echo
    echo
    echo "Building Expat for iOS..."
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
                                
                cd $DIR/_deps/libexpat-src/expat
                ./buildconf.sh
                                
                PREFIX=$DIR/_deps/libexpat-build/iOS/$build_mode/$ARCH

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
                    export CFLAGS="$CFLAGS -g -O2 -DNDEBUG"
                fi
                
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

#################################### Building for iOS-Simulator ####################################
if [ $BYPASS_IOS_SIMULATOR == 0 ]
then

     echo
     echo
     echo "Building Expat for iOS-Simulator..."
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

                cd $DIR/_deps/libexpat-src/expat
                ./buildconf.sh
                                
                PREFIX=$DIR/_deps/libexpat-build/iOS-Simulator/$build_mode/$ARCH

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
