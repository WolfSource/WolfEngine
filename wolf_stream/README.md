# Wolf Stream
wolf_stream contains all streaming features of Wolf, this library is developed based on FFMPEG, GStreamer and LIVE555

## Installation

To build this library, first you need to have at least GStreamer 1.8 and gst-plugins-base 1.8 installed. In addition, you might require various GStreamer plugins to be
available, which can be found in gst-plugins-base, gst-plugins-good,
gst-plugins-bad, gst-plugins-ugly and/or gst-libav.

<a name="installation-windows"/>

### Windows

You need to download the *two* `.msi` files for your platform from the
[GStreamer website](https://gstreamer.freedesktop.org/data/pkg/windows/) provided by
the GStreamer project and install them, e.g. `gstreamer-1.0-x86_64-x.y.z.msi` and
`gstreamer-1.0-devel-x86_64-x.y.z.msi`.

After installation, you also need to install `pkg-config` (e.g. via MSYS2 or
from [here](https://sourceforge.net/projects/pkgconfiglite/))
and set the `PKG_CONFIG_PATH` environment variable

```console
$ export PKG_CONFIG_PATH="c:\\gstreamer\\1.0\\x86_64\\lib\\pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
```

<a name="installation-linux"/>

### Linux/BSDs

You need to install the above mentioned packages with your distributions
package manager, or build them from source.

On Debian/Ubuntu they can be installed with

```console
$ apt-get install libssl-dev gcc g++ make cmake \
      libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
      gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
      gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
      gstreamer1.0-libav libgstrtspserver-1.0-dev libges-1.0-dev
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

