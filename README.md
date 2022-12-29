# Wolf Engine [![Apache licensed](https://img.shields.io/badge/license-Apache-blue)](https://github.com/WolfEngine/Wolf.Engine/blob/main/LICENSE.md) [![codecov](https://codecov.io/github/WolfEngine/WolfEngine/branch/main/graph/badge.svg?token=AhoU9QV7eS)](https://codecov.io/github/WolfEngine/WolfEngine) [![CodeQL](https://github.com/WolfEngine/WolfEngine/actions/workflows/codeql.yml/badge.svg?branch=main)](https://github.com/WolfEngine/WolfEngine/actions/workflows/codeql.yml) [![Microsoft C++ Code Analysis](https://github.com/WolfEngine/WolfEngine/actions/workflows/msvc.yml/badge.svg?branch=main)](https://github.com/WolfEngine/WolfEngine/actions/workflows/msvc.yml)

<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/Logo.png" width="256" height="256" alt="WolfEngine"/>
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://github.com/PooyaEimandar/PersianEngine">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a href="https://pooyaeimandar.github.io/">Pooya Eimandar</a>.
The Wolf is a comprehensive set of C++ open source libraries for realtime rendering, realtime streaming and game developing, which is support <b>Lua</b> and <b>WASM</b> as an embedded scripting languages.</p>

# Build
- Prerequisites 
      - For windows, make sure install the latest Windows 11/10 SDK
	- [git](https://git-scm.com/downloads)
	- [CMake](https://cmake.org/download/)
	- [Meson](https://github.com/mesonbuild/meson/releases)
	- optional: [Ninja](https://ninja-build.org/). Alternatively, use "pip3 install ninja"
	- [Nasm](https://nasm.us/)
	- [Perl](https://www.perl.org/get.html) for boringSSL. [Strawberry Perl](https://strawberryperl.com/) is recommended for Windows.
	- [Go](https://go.dev/dl/) for boringSSL
	- [QT6](https://www.qt.io/download) for wolf render, demos and examples
	
then make sure get the main branch 
`git clone https://github.com/WolfEngine/WolfEngine.git --branch main --depth 1`

## CMakePresets
	
To list configure presets: `cmake . --list-presets`
To list build presets: `cmake --build --list-presets`

### Android
In order to configure, build and test using CMakePresets you must have set the following environment variables:
 - NDK (e.g. C:\Android\android-ndk-r25b)
 
In terminal/command line use presets:
```
cmake . --preset android-arm64-release
cmake --build --preset default-build-android
```

## Recent Sample
<p>Dynamic LOD Generation using <a href="https://www.simplygon.com/" target="_blank">Simplygon</a></p>
<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/wolf-2/samples/03_advances/07_lod/doc/view.gif" width="640" height="360" alt="Dynamic LOD Generation gif"/>

## Supported platforms

| Planned | In Progress | Done |
|:-----------:|:-----------:|:-----------:|
| :memo:  | :construction: | :white_check_mark: | 

### Supported platforms and APIs for render module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
|  QT | Vulkan/OpenGL ES :construction: | Vulkan/OpenGL ES :memo: | Metal :memo: | Metal :memo: | Vulkan/OpenGL ES :memo: | WebGL:memo: |

### Supported platforms and APIs for media module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| FFmpeg | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| JPEG | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| OpenAL | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| PNG | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| WebP | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |

### Supported platforms and APIs for stream module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| gRPC | :memo: | :x: | :x: | :x: | :x: | :x: |
| QUIC | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| RIST | :white_check_mark: | :memo: | :memo: | :memo: | :white_check_mark: | :x: |
| RTMP | :memo: | :x: | :x: | :x: | :x: | :x: |
| RTSP | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| SRT | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| webRTC | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| WebSocket | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :memo: |


### Supported platforms and APIs for system module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| Fiber | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| Gamepad | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :white_check_mark: |
| Virtual Gamepad | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| Log  | :white_check_mark: | :construction: | :construction: | :construction: | :construction: | :construction: | 
| LuaJit  | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| LZ4  | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| LZMA  | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| OpenTelemetry  | :memo: | :memo: | :memo: | :x: | :x: | :x: |
| RAFT  | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| Screen Capture  | :memo: | :construction: | :construction: | :x: | :x: | :x: |
| Signal Slot  | :white_check_mark: | :construction: | :construction: | :x: | :x: | :x: |
| Stacktrace  | :white_check_mark: | :construction: | :construction: | :construction: | :construction: | :x: |
| Socket | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| Sycl  | :memo: | :memo: | :memo: | :x: | :x: | :x: |
| Trace | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| Wasm3  | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |

## Projects using Wolf</h2>
* [Wolf.Playout](https://www.youtube.com/watch?v=EZSdEjBvuGY), a playout automation software
* [Falcon](https://youtu.be/ygpz35ddZ_4), a real time 3D monitoring system
* [PlayPod](https://playpod.ir), the first cloud gaming platform launched in Middle East
* [RivalArium](https://rivalarium.com), play and rival other users via our leagues and duels from any device, any location and let your skills generate income

## [Youtube](https://www.youtube.com/c/WolfEngine)
## [Twitter](https://www.twitter.com/Wolf_Engine)

Wolf Engine © 2014-2022 [Pooya Eimandar](https://www.linkedin.com/in/pooyaeimandar/)
