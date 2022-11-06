# Wolf Engine [![Apache licensed](https://img.shields.io/badge/license-Apache-blue)](https://github.com/WolfEngine/Wolf.Engine/blob/main/LICENSE.md) [![Build status](https://ci.appveyor.com/api/projects/status/nrk0kn83tp1n47h3/branch/master?svg=true)](https://ci.appveyor.com/project/PooyaEimandar/wolf-engine/branch/master)

<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/Logo.png" width="256" height="256" alt="WolfEngine"/>
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://github.com/PooyaEimandar/PersianEngine">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a href="https://pooyaeimandar.github.io/">Pooya Eimandar</a>.
The Wolf is a comprehensive set of C++ open source libraries for realtime rendering, realtime streaming and game developing, which is support <b>Lua</b> and <b>WASM</b> as an embedded scripting languages.</p>

# Build
- Install 
      - For windows, make sure install the latest Windows 11/10 SDK
	- [git](https://git-scm.com/downloads)
	- [CMake](https://cmake.org/download/)
	- [Meson](https://github.com/mesonbuild/meson/releases)
	- optional: [Ninja](https://ninja-build.org/). Alternatively, use "pip3 install ninja"
	- [Nasm](https://nasm.us/)
	- [Perl](https://www.perl.org/get.html) for boringSSL. [Strawberry Perl](https://strawberryperl.com/) is recommended for Windows.
	- [Go](https://go.dev/dl/) for boringSSL

### Recent Sample
<p>Dynamic LOD Generation using <a href="https://www.simplygon.com/" target="_blank">Simplygon</a></p>
<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/wolf-2/samples/03_advances/07_lod/doc/view.gif" width="640" height="360" alt="Dynamic LOD Generation gif"/>

### Supported platforms and APIs for render module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
|  Vulkan | :construction: | :memo: | :memo: | :memo: | :x: | :x: |
|  OpenGL | :memo: | :memo: | :x: | :x: | :x: | :x: |
|  WebGPU | :x: | :x: | :x: | :x: | :x: | :memo: |
|  WebGL  | :x: | :x: | :x: | :x: | :x: | :memo: |

### Supported platforms and APIs for media module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| JPEG | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| WebP | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| PNG | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |

### Supported platforms and APIs for stream module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| RIST | :construction: | :memo: | :memo: | :memo: | :memo: | :x: |
| RTSP | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| RTMP | :memo: | :x: | :x: | :x: | :x: | :x: |
| webRTC | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| QUIC | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| WS | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |

### Supported platforms and APIs for system module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| Socket | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| Fiber | :construction: | :memo: | :memo: | :x: | :x: | :x: |
| Trace | :construction: | :memo: | :memo: | :memo: | :memo: | :x: |
| Signal-Slot  | :construction: | :construction: | :construction: | :construction: | :construction: | :construction: |
| LuaJit  | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| WASM3  | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| LZ4  | :construction: | :memo: | :memo: | :memo: | :memo: | :x: |
| LZMA  | :construction: | :memo: | :memo: | :x: | :x: | :x: |
| Log  | :construction: | :construction: | :construction: | :construction: | :construction: | :construction: |


### Projects using Wolf</h2>
* [Wolf.Playout](https://www.youtube.com/watch?v=EZSdEjBvuGY), a playout automation software
* [Falcon](https://youtu.be/ygpz35ddZ_4), a real time 3D monitoring system
* [PlayPod](https://playpod.ir), the first cloud gaming platform launched in Middle East
* [RivalArium](https://rivalarium.com), play and rival other users via our leagues and duels from any device, any location and let your skills generate income

### [Youtube](https://www.youtube.com/c/WolfEngine)
### [Twitter](https://www.twitter.com/Wolf_Engine)

Wolf Engine © 2014-2022 [Pooya Eimandar](https://www.linkedin.com/in/pooyaeimandar/)
