#! /bin/bash

display_help() {
    echo "Use:"
    echo "     --debug                  build in debug mode"
    echo
    echo "     --release                build in release mode"
    echo
    echo "     --build-dir              the build directory"
    echo
    exit 1
}

open_or_create_dir()
{
  if [ ! -d "$1" ]; then
    mkdir $1
    if [ ! -d "$1" ]; then
      echo "ERROR, could not make dir $1"
      exit
    fi
  fi
}

check_for_package()
{
    REQUIRED_PKG="$1"
    PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
    
	if rpm -q $REQUIRED_PKG
	then
	    PKG_OK="$REQUIRED_PKG"
	else
	    PKG_OK=""
	fi
}

BUILD_MODES="Debug"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

################################
# Check if parameters options  #
# are given on the commandline #
################################
while [ "$1" != "" ]; do
    case $1 in
   	--build_dir=* )             DIR="${1#*=}"
                                    shift 0
                                    ;;
        --Debug )                   BUILD_MODES="Debug"
                                    shift 0
                                    ;;
        --Release )                 BUILD_MODES="Release"
                                    shift 0
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

REQUIRED_PKG="apt-get yum"
package=""

for package_type in $REQUIRED_PKG
do
   echo "Checking for $package_type"
   check_for_package $package_type
   if [ "$PKG_OK" != "" ]; 
   then
      package=$package_type
      break
   fi 
done

if [ "$package" = "" ]; 
then
  echo "No package found for getting required Unix packages"
  exit
fi

case $package in
     "apt-get" )                 sudo $package install -y git autoconf libtool libapr1 libapr1-dev libssl-dev
                                 ;;
     "yum" )                     sudo $package install -y git autoconf libtool apr1 apr-devel openssl-devel
                                 ;;
     * )                         echo "Package not found."
                                 exit
                                 ;;
esac

cd $DIR
DEPS="$DIR/_deps/"
open_or_create_dir $DEPS
cd $DEPS
open_or_create_dir "$DEPS/activemq-cpp-build/"
cd "$DEPS/activemq-cpp-build/"
BUILD=$DEPS/activemq-cpp-build/$BUILD_MODES
open_or_create_dir $BUILD

# now clone activemq-cpp
cd $DEPS
git clone https://github.com/apache/activemq-cpp.git
cd ./activemq-cpp/activemq-cpp
sudo chmod +x ./autogen.sh
./autogen.sh
./configure --prefix=$BUILD 
make install
