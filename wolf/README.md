# Wolf Engine [![LGPL v3 licensed](https://img.shields.io/badge/license-Apache-blue)](https://github.com/WolfEngine/Wolf.Engine/blob/main/LICENSE.md)
<img src="https://raw.githubusercontent.com/WolfEngine/WolfEngine/main/Logo.png" width="256" height="256" alt="WolfEngine"/>

**Wolf Engine** is the next generation of [Persian Game Engine](https://github.com/PooyaEimandar/PersianEngine) which is a
cross-platform open source game engine created by [Pooya Eimandar](https://pooyaeimandar.github.io)
This Wolf is a comprehensive set of Rust/C libraries for realtime rendering, realtime streaming and game developing, which is support **Lua** & **Python** as an embedded script and binding language.</p>

## Projects using Wolf
- [Wolf.Playout](https://www.youtube.com/watch?v=EZSdEjBvuGY) is a playout automation software
- [Falcon](https://youtu.be/ygpz35ddZ_4) is a real time 3D monitoring system, developed at [FANAP Co.](https://fanap.ir/)
- Barf is a real time streaming core for cloud gaming platform

## Branches
- [main](https://github.com/WolfEngine/WolfEngine/tree/main), Wolf3, is the latest version of Wolf which is written in **Rust and contains some unsafe codes** and is not ready for production
- [Wolf2](https://github.com/WolfEngine/WolfEngine/tree/wolf-2) is written in **C/C++ and is in maintenance mode**
- [releases](https://github.com/WolfEngine/WolfEngine/releases) contains old releases and source codes

## Build
- **Wolf 2/1** via CMake
- **Wolf 3**
  - For **Webassembly** :\
  First you need to setup wasm32
  ```bash
  rustup target add wasm32-unknown-unknown
  ```
  Now run hello example from root of workspace
  ```bash
  cargo run-wasm --example hello
  ```
  Finally the example will be served at http://localhost:8000
  - For **Native** :
  ```bash
  cargo run --example hello
  ```
## Copyright & License
Wolf Engine © 2014-2022 [Pooya Eimandar](https://www.linkedin.com/in/pooyaeimandar)