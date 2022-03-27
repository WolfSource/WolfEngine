# Extra Command

```shell
$ rustup target add \
    aarch64-unknown-linux-musl \
    armv7-unknown-linux-musleabihf \
    x86_64-unknown-linux-musl
$ cargo build --package wolfengine --release --target aarch64-unknown-linux-musl
```

```shell
$ rustc --print target-list
$ rustc --print target-cpus --target ${TRIPLE}
$ rustc --print target-features --target ${TRIPLE}
```

```shell
$ docker build . --file ./wolf/Dockerfile --tag wolf/system:0.1.0-nonroot
$ docker run --env LINKERD_AWAIT_DISABLED=TRUE --interactive --publish 8080:8080 --rm wolf/system:0.1.0-nonroot
```

```shell
$ brew install upx
$ upx --ultra-brute ./target/aarch64-unknown-linux-musl/release/wolf
```

```shell
$ perf record --call-graph dwarf ./target/aarch64-unknown-linux-musl/release/wolf
$ perf report --disassembler-style intel --hierarchy
```

```shell
$ export CARGO_INCREMENTAL=0
$ export RUSTDOCFLAGS="-Cpanic=abort"
$ export RUSTFLAGS="-Zprofile -Ccodegen-units=1 -Copt-level=0 -Clink-dead-code -Coverflow-checks=off -Zpanic_abort_tests -Cpanic=abort"
```
