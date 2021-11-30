# CREATE

```shell
$ cargo --list
$ cargo -Z help

$ cargo bench
$ cargo build --release
$ cargo check
$ cargo clean
$ cargo doc
$ cargo fetch
$ cargo fix
$ cargo generate-lockfile
$ cargo init

$ cargo new
$ cargo package
$ cargo run
$ cargo search
$ cargo test
$ cargo tree
$ cargo update
$ cargo vendor

$ cargo clippy
$ cargo fmt
$ cargo miri

$ cargo install --list
$ cargo install --force --locked \
    bunyan \
    cargo-audit \
    cargo-cache \
    cargo-deny \
    cargo2junit \
    conventional_commits_linter \
    cross \
    grcov \
    sqlx-cli
$ cargo uninstall

$ cargo audit
$ cargo deny
$ cargo grcov
```

## Project

```shell
$ cargo new --bin rust_bin_test
$ cargo new --lib rust_lib_test

$ cargo doc --open
```

## Run

```shell
$ cargo bench
$ cargo cache --autoclean
$ cargo check
$ cargo doc
$ cargo fix
$ cargo package
$ cargo publish
$ cargo run
$ cargo rustc
$ cargo test
$ cargo update
$ cargo upgrade --workspace
$ cargo vendor
```

## Benchmark

```shell
$ cargo bench -- --save-baseline before
$ cargo bench -- --save-baseline change
$ critcmp before change
```
