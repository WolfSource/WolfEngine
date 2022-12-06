## Build
- **Wolf 2/1** via CMake
- **Wolf 3 via Nightly toolchain**
  ```bash
  rustup default nightly
  ```
  - For **Webassembly** :\
  First you need to setup wasm32
  ```bash
  rustup target add wasm32-unknown-unknown
  ```
  Now run hello wolf demo
  ```bash
  ./build-wasm.sh
  ./run-wasm.sh
  ```
  Finally the demo will be served at http://localhost:8000
  - For **Native** :
  ```bash
  cd wolf-demo
  cargo run
  ```
## Copyright & License
Wolf Engine © 2014-2022 [Pooya Eimandar](https://www.linkedin.com/in/pooyaeimandar)