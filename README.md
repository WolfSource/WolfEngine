# Wolf Engine [![LGPL v3 licensed](https://img.shields.io/badge/license-Apache-blue)](https://github.com/WolfEngine/Wolf.Engine/blob/main/LICENSE.md) [![coverage](https://shields.io/endpoint?url=https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/coverage/coverage.json)](https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/coverage/coverage.json) [![wakatime](https://wakatime.com/badge/github/WolfEngine/WolfEngine.svg)](https://wakatime.com/badge/github/WolfEngine/WolfEngine)

<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/Logo.png" width="256" height="256" alt="WolfEngine"/>

**Wolf Engine** is the next generation of [Persian Game Engine](https://github.com/PooyaEimandar/PersianEngine) which is a
cross-platform open source game engine created by [Pooya Eimandar](https://pooyaeimandar.github.io)
This Wolf is a comprehensive set of Rust/C libraries for realtime rendering, realtime streaming and game developing, which is support **Lua** & **Python** as an embedded script and binding language.</p>

## Recent Sample
<p>Dynamic LOD Generation using <a href="https://www.simplygon.com/" target="_blank">Simplygon</a></p>
<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/wolf-2/samples/03_advances/07_lod/doc/view.gif" width="640" height="360" alt="Dynamic LOD Generation gif"/>

### Supported platforms and APIs for render module
| Planned | In Progress | Done |
|:-----------:|:-----------:|:-----------:|
| :memo:  | :construction: | :white_check_mark: | 

| API | Windows | Linux | macOS | iOS | Android | Wasm(WebGPU) | Wasm(WebGL) |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|:-------------:|
|  WebGPU | :construction: | :construction: | :construction: | :memo: | :memo: | :construction: | :construction: |

### Supported platforms and APIs for media module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| FFmpeg | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| OpenAL | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |

### Supported platforms and APIs for stream module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| gRPC | :white_check_mark: | :x: | :x: | :x: | :x: | :x: |
| QUIC | :construction: | :memo: | :memo: | :memo: | :memo: | :x: |
| RIST | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| RTMP | :memo: | :x: | :x: | :x: | :x: | :x: |
| RTSP | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| SRT | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| webRTC | :memo: | :memo: | :memo: | :memo: | :memo: | :memo: |
| WS | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :memo: |

### Supported platforms and APIs for system module

| API | Windows | Linux | macOS | iOS | Android | Wasm |
|:-----------:|:-----------:|:--------------------------:|:--------------:|:-------------:|:--------------:|:-------------:|
| Embedded JavaScript  | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :memo: |
| Fiber | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| Gamepad | :construction: | :memo: | :memo: | :memo: | :memo: | :memo: |
| Gamepad Simulator | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| Log  | :construction: | :construction: | :construction: | :construction: | :construction: | :construction: | 
| LuaJit  | :memo: | :memo: | :memo: | :memo: | :memo: | :x: |
| LZ4  | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| LZMA  | :white_check_mark: | :memo: | :memo: | :x: | :x: | :x: |
| OpenTelemetry  | :memo: | :memo: | :memo: | :x: | :x: | :x: |
| RAFT  | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :memo: |
| Screen Capture  | :white_check_mark: | :construction: | :construction: | :x: | :x: | :x: |
| Signal-Slot  | :white_check_mark: | :construction: | :construction: | :construction: | :construction: | :white_check_mark: |
| Stacktrace  | :white_check_mark: | :construction: | :construction: | :construction: | :construction: | :x: |
| Socket | :white_check_mark: | :memo: | :memo: | :memo: | :memo: | :x: |
| Trace | :construction: | :memo: | :memo: | :memo: | :memo: | :x: |
| Wasmer  | :memo: | :memo: | :memo: | :x: | :x: | :x: |

## Branches
- [main](https://github.com/WolfEngine/WolfEngine/tree/main), Wolf3, is the latest version of Wolf which is written in **Rust and contains some unsafe cpp codes** and is not ready for production
- [Wolf2](https://github.com/WolfEngine/WolfEngine/tree/wolf-2) is written in **C and is in maintenance mode**
- [releases](https://github.com/WolfEngine/WolfEngine/releases) contains old releases and source codes (C++ & DirectX 11)

## Build
- Current **Wolf** via CMake
- **Wolf-rs**
  - Install [CMake](https://cmake.org/install/)
  - Install [Meson](https://github.com/mesonbuild/meson/releases)
  - Install [clang](https://github.com/llvm/llvm-project/releases/tag/llvmorg-14.0.0) for bindgen
  - Install [python3](https://www.python.org/downloads/) for running wasm
  - Install protoc for 
    - Windows: download protoc-x.y-win64.zip from [here](https://github.com/protocolbuffers/protobuf/releases/latest) then extract the file `bin\protoc.exe` and put it somewhere in the `PATH`
    - MacOS:
      ```bash
        brew install protobuf
      ```  
    - Linux:
      ```bash
        sudo apt update && sudo apt upgrade -y
        sudo apt install -y protobuf-compiler libprotobuf-dev
      ```

  - Install prerequisites of **Webassembly**:\
  From workspace folder run
  ```bash
  cargo install wasm-pack
  rustup default nightly
  rustup target add wasm32-unknown-unknown
  cd wolf-demo
  ./build-wasm.sh
  ./run-wasm.sh
  ```
  Finally the demo will be served at http://localhost:8000
  - For **Windows, MacOS, Linux** :
  ```bash
  rustup default stable
  cd wolf-demo
  cargo run
  ```
  - For **Android** :
  ```bash
  rustup default nightly
  rustup target add \
    aarch64-linux-android \
    armv7-linux-androideabi \
    x86_64-linux-android \
    i686-linux-android
  cargo install cargo-ndk
  export ANDROID_NDK_HOME = /path/to/the/root/of/NDK/Folder
  cargo ndk -t armeabi-v7a build
  cargo ndk -t armeabi-v7a -o ./jniLibs build --release 
  ```

  - For **iOS** :
  ```bash
  rustup default stable
  rustup target add \
    aarch64-apple-ios \
    x86_64-apple-ios
  cargo install cargo-lipo
  cd wolf
  cargo lipo --release
  ```

## Copyright & License
Wolf Engine © 2014-2022 [Pooya Eimandar](https://www.linkedin.com/in/pooyaeimandar)
