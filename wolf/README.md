# Wolf Engine [![LGPL v3 licensed](https://img.shields.io/badge/license-Apache-blue)](https://github.com/WolfEngine/Wolf.Engine/blob/main/LICENSE.md) [![coverage](https://shields.io/endpoint?url=https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/coverage/coverage.json)](https://github.com/WolfEngine/WolfEngine/tree/main/coverage/index.html) [![wakatime](https://wakatime.com/badge/github/WolfEngine/WolfEngine.svg)](https://wakatime.com/badge/github/WolfEngine/WolfEngine)

<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/Logo.png" width="256" height="256" alt="WolfEngine"/>

**Wolf Engine** is the next generation of [Persian Game Engine](https://github.com/PooyaEimandar/PersianEngine) which is a
cross-platform open source game engine created by [Pooya Eimandar](https://pooyaeimandar.github.io)
This Wolf is a comprehensive set of Rust/C libraries for realtime rendering, realtime streaming and game developing, which is support **Lua** & **Python** as an embedded script and binding language.</p>

## Branches
- [main](https://github.com/WolfEngine/WolfEngine/tree/main), Wolf3, is the latest version of Wolf which is written in **Rust and contains some unsafe codes** and is not ready for production
- [Wolf2](https://github.com/WolfEngine/WolfEngine/tree/wolf-2) is written in **C/C++ and is in maintenance mode**
- [releases](https://github.com/WolfEngine/WolfEngine/releases) contains old releases and source codes (C++ & DirectX 11)

## Build
- **Wolf 2/1** via CMake
- **Wolf 3**
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
