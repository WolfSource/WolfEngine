# Wolf System
wolf_system is the base library which contains all streaming features of Wolf Engine, this library is developed based on FFMPEG, GStreamer and LIVE555

## Installation

cd ./path-to-the-root-wolf_system
cargo build --release

<a name="installation-windows"/>

### Windows

You need to download and install latest Visual Studio with MSVC

<a name="installation-linux"/>

### Linux/BSDs

You need to install the above mentioned packages with your distributions
package manager, or build them from source.

On Debian/Ubuntu they can be installed with

```console
$ apt-get install libssl-dev gcc g++ make cmake
```

The minimum required version of the above libraries is >= 1.8. If you
build the gstreamer-player sub-crate, or any of the examples that
depend on gstreamer-player, you must ensure that in addition to the
above packages, `libgstreamer-plugins-bad1.0-dev` is installed and
that the version is >= 1.12. See the `Cargo.toml` files for the full
details,

```console
$ # Only if you wish to install gstreamer-player, make sure the version
$ # of this package is >= 1.12.
$ apt-get install libgstreamer-plugins-bad1.0-dev
```

Package names on other distributions should be similar.
Please submit a pull request with instructions for yours.

<a name="installation-macos"/>

### macOS

You need to download the *two* `.pkg` files from the [GStreamer website](https://gstreamer.freedesktop.org/data/pkg/osx/) and install them, e.g. `gstreamer-1.0-x.y.z-x86_64.pkg` and
`gstreamer-1.0-devel-x.y.z-x86_64.pkg`.

After installation, you also need to install `pkg-config` (e.g. via [Homebrew](https://brew.sh/))
and set the `PKG_CONFIG_PATH` environment variable

```console
$ open ~/.zprofile
$ export PKG_CONFIG_PATH="/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
$ source .zprofile
```

