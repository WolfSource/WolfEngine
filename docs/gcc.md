cd /home/build
GCC_VERSION=11.2.0
wget https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz
tar xzvf gcc-${GCC_VERSION}.tar.gz
mkdir obj.gcc-${GCC_VERSION}
cd gcc-${GCC_VERSION}
./contrib/download_prerequisites
cd ../obj.gcc-${GCC_VERSION}
#RedHat base
sudo yum groupinstall "Development Tools"
#Debian
sudo apt-get install build-essential
../gcc-${GCC_VERSION}/configure --disable-multilib --enable-languages=c,c++
make -j $(nproc)
make install
