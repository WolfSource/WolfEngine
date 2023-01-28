#!/bin/bash
# any case there is an issue, check this "https://github.com/meetecho/janus-gateway"

echo "setup the prerequisites"
sudo apt-get install meson libmicrohttpd-dev libjansson-dev libssl-dev libsrtp2-dev libsofia-sip-ua-dev libglib2.0-dev libopus-dev libogg-dev libcurl4-openssl-dev liblua5.3-dev libconfig-dev pkg-config libtool automake
echo "installing libnice"
git clone https://gitlab.freedesktop.org/libnice/libnice
cd libnice
meson --prefix=/usr build && ninja -C build && sudo ninja -C build install
cd ..
wget https://github.com/cisco/libsrtp/archive/v2.2.0.tar.gz
tar xfv v2.2.0.tar.gz
cd libsrtp-2.2.0
./configure --prefix=/usr --enable-openssl
make shared_library && sudo make install
echo "installing boringssl"
cd ..
git clone https://boringssl.googlesource.com/boringssl
cd boringssl
# don't barf on errors
sed -i s/" -Werror"//g CMakeLists.txt
mkdir -p build
cd build
cmake -DCMAKE_CXX_FLAGS="-lrt" ..
make
cd ..
sudo mkdir -p /opt/boringssl
sudo cp -R include /opt/boringssl/
sudo mkdir -p /opt/boringssl/lib
sudo cp build/ssl/libssl.a /opt/boringssl/lib/
sudo cp build/crypto/libcrypto.a /opt/boringssl/lib/
echo "installing usrsctp"
cd ..
git clone https://github.com/sctplab/usrsctp
cd usrsctp
./bootstrap
./configure --prefix=/usr --disable-programs --disable-inet --disable-inet6
make && sudo make install
echo "installing nanomsg"
cd ..
aptitude install libnanomsg-dev
sh autogen.sh
./configure --prefix=/usr/local/janus PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig --enable-nanomsg --enable-data-channels
sudo make install
sudo make configs
echo "all done."
echo "you can change the config of \'janus.transport.http.jcfg\' from \'/usr/local/janus/etc/janus/janus.transport.http.jcfg\' and change the following \n
\n
general: {
	https = true					# Whether to enable HTTPS 
	secure_port = 8089				# Web server HTTPS port, if enabled
}
\n
certificates: {
	cert_pem = "/path/to/the/cert.pem"
	cert_key = "/path/to/the/cert.key"
}
\n"
echo "running Janus"
cd /usr/local/janus/bin 
sudo janus