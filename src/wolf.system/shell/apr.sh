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

    git clone https://github.com/apache/apr.git #-b master
    mv "$DIR/_deps/apr" "$DIR/_deps/apr-src"
}

#################################### INIT ####################################################
BUILD_DARWIN_VER=`uname -r`
BYPASS_IOS=0
BYPASS_MACOS=0
BYPASS_IOS_SIMULATOR=0
BYPASS_LINUX=0
BYPASS_TESTS=0
IS_EXPAT_LIB=1
IS_EXPAT_INCLUDE=1
BUILD_MODES="Debug"
TARGET=""
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SHELL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

################################
# Check if parameters options  #
# are given on the commandline #
################################
while [ "$1" != "" ]; do
    case $1 in
        --bypass_macos=* )          BYPASS_MACOS="${1#*=}"
                                    shift 7
                                    ;;
        --bypass_ios=* )            BYPASS_IOS="${1#*=}"
                                    shift 7
                                    ;;
        --bypass_ios_simulator=* )  BYPASS_IOS_SIMULATOR="${1#*=}"
                                    shift 7
                                    ;;
        --bypass_linux=* )          BYPASS_LINUX="${1#*=}"
                                    shift 7
                                    ;;
        --without-test=* )          BYPASS_TESTS="${1#*=}"
                                    shift 7
                                    ;;
        --Debug )                   BUILD_MODES="Debug"
                                    shift 7
                                    ;;
        --Release )                 BUILD_MODES="Release"
                                    shift 7
                                    ;;
        --build_dir=* )             DIR="${1#*=}"
                                    shift 7
                                    ;;
        --arch=* )                  TARGET="${1#*=}"
                                    shift 7
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

echo $TARGET

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

    if [[ -f "/usr/lib/libxml2.a" ]] || [[ -f "/usr/lib/libxml2.dylib" ]]
    then
        return
    fi

    if [[ ! -f "/usr/lib/libexpat.a" ]]
    then
        if [[ ! -f "$DIR/_deps/libexpat-build/$1/libexpat.a" ]] && [[ ! -f "$DIR/_deps/libexpat-build/libexpat.a" ]] && [[ ! -f "$DIR/_deps/libexpat-build/lib/libexpat.a" ]]
        then
            echo "Missing a file! Build libexpat and then try again."
            echo "Building APR failed. Returning..."
            exit
        else
            IS_EXPAT_LIB=0
            sudo cp "$DIR/_deps/libexpat-build/$1/libexpat.a" "/usr/lib"
            sudo cp "$DIR/_deps/libexpat-build/libexpat.a" "/usr/lib"
            sudo cp "$DIR/_deps/libexpat-build/lib/libexpat.a" "/usr/lib"
            sudo cp "$DIR/_deps/libexpat-build/expat.pc" "/usr/lib"
        fi
    else
        IS_EXPAT_LIB=0
        sudo cp "/usr/lib/libexpat.a" "/usr/lib"
        sudo cp "$DIR/_deps/libexpat-build/expat.pc" "/usr/lib"
    fi

    #Check for libexpat include files
    if [[ ! -f "/usr/include/expat_config.h" ]]
    then
        IS_EXPAT_INCLUDE=0
        sudo cp "$DIR/_deps/libexpat-build/expat_config.h" "/usr/include"
        sudo cp "$DIR/_deps/libexpat-src/expat/lib/expat.h" "/usr/include"
        sudo cp "$DIR/_deps/libexpat-src/expat/lib/expat_external.h" "/usr/include"
    fi
}

mkdir -p "$DIR/_deps/"
extract_apr_trunk

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
if [ $BYPASS_MACOS == 1 ] && [ $BYPASS_LINUX == 1 ]
then
    if [[ ! -f "$SHELL_DIR/tools/gen_test_char" ]]
    then
        echo "Missing a file!"
        echo "Building APR failed. Returning..."
        exit
    fi
    cp "$SHELL_DIR/tools/gen_test_char" "$DIR/_deps/apr-src/tools"
fi

#################################### Building for MacOS ########################################
if [ $BYPASS_MACOS == 0 ]
then

    echo
    echo
    echo "Building APR for MacOS..."
    echo
    echo
        
    if [ "$TARGET" = "" ]
    then
        ARCHS="x86_64"
    else
        ARCHS="$TARGET"
    fi
    
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
                
                if [ $IS_EXPAT_LIB == 0 ]
                then
                    CONFIGURE_FLAGS="$CONFIGURE_FLAGS --with-expat=$DIR/_deps/libexpat-build"
                fi
                
                ./configure \
                    --prefix=$PREFIX \
                    --host=$TARGET \
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
    
    echo "TARGET   $TARGET"
    
    if [ "$TARGET" = "" ]
    then
        ARCHS="arm64 armv7 armv7s"
    else
        ARCHS="$TARGET"
    fi
    
    echo "ARCHS   $ARCHS"
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                                      
                check_libexpat "${build_mode}" $ARCH

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

                if [ $IS_EXPAT_LIB == 0 ]
                then
                    CONFIGURE_FLAGS="$CONFIGURE_FLAGS --with-expat=$DIR/_deps/libexpat-build"
                fi
                
                ./configure \
                    --prefix="$PREFIX" \
                    --host=$TARGET \
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
                
                cp -R "$DIR/_deps/apr-build/$build_mode/$ARCH/lib" "$DIR/_deps/apr-build/$build_mode/"

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
          
    if [ "$TARGET" = "" ]
    then
        ARCHS="i386 x86_64"
    else
        ARCHS="$TARGET"
    fi

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
                
                if [ $IS_EXPAT_LIB == 0 ]
                then
                    CONFIGURE_FLAGS="$CONFIGURE_FLAGS --with-expat=$DIR/_deps/libexpat-build"
                fi

                ./configure \
                    --prefix="$PREFIX" \
                    --host=$TARGET \
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
        
    if [ "$TARGET" = "" ]
    then
        ARCHS="x86_64"
    else
        ARCHS="$TARGET"
    fi
    
    for build_mode in $BUILD_MODES
    do
            for ARCH in $ARCHS
            do
            
                echo "building $ARCH..."
                
                if [ \( -d "$DIR/linux/$build_mode" \) ]
                then
                    rm -r "$DIR/linux/$build_mode"
                fi
                
                check_libexpat $build_mode $ARCH
                
                cd $DIR/_deps/apr-src
                ./buildconf
                
                PREFIX="$DIR/_deps/apr-build/$build_mode"
                CONFIGURE_FLAGS=""
                TARGET=""
                                
                if [ "$build_mode" = "Debug" ]
                then
                    export CFLAGS="$CFLAGS -g -O0 -DDEBUG -fPIC"
                fi
                
                if [ "$build_mode" = "Release" ]
                then
                    export CFLAGS="$CFLAGS -g -O3 -DNDEBUG -fPIC"
                fi
                
                CONFIGURE_FLAGS="--enable-static --enable-shared"
                                
                if [[ -f "$DIR/_deps/libexpat-build/expat_config.h" ]] 
                then
                	mkdir $DIR/_deps/libexpat-build/include
                 	cp $DIR/_deps/libexpat-build/expat_config.h $DIR/_deps/libexpat-build/include
                fi
                
                if [[ -f "$DIR/_deps/libexpat-build/libexpat.a" ]] 
                then
                	mkdir $DIR/_deps/libexpat-build/lib
                	cp $DIR/_deps/libexpat-build/libexpat.a $DIR/_deps/libexpat-build/lib
                fi
                
                if [ $IS_EXPAT_LIB == 0 ]
                then
                    CONFIGURE_FLAGS="$CONFIGURE_FLAGS --with-expat=$DIR/_deps/libexpat-build"
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
#################################### Finish ####################################
if [ $IS_EXPAT_LIB == 0 ]
then
    if [[ -f "/usr/lib/libexpat.a" ]]
    then
        rm "/usr/lib/libexpat.a"
    fi
fi

if [ $IS_EXPAT_INCLUDE == 0 ]
then
    if [ \( -d "/usr/include/expat_config.h" \) ]
    then
        rm -r "/usr/include/expat_config.h"
    fi
fi