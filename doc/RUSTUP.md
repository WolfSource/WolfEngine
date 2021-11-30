# RUSTUP

```shell
$ curl \
        --fail \
        --location \
        --proto "=https" \
        --show-error \
        --silent \
        --tlsv1.2 https://sh.rustup.rs \
    | sh -s -- -y

$ rustup show

$ rustup default

$ rustup toolchain list
$ rustup toolchain install nightly

$ rustup target list
$ rustup target add aarch64-apple-darwin

$ rustup component list
$ rustup +nightly component add \
    cargo \
    clippy \
    llvm-tools-preview \
    miri \
    rls \
    rust-analysis \
    rust-analyzer-preview \
    rust-docs \
    rust-src \
    rust-std \
    rustc \
    rustc-dev \
    rustfmt

$ rustup self update \
    && rustup update \
    && rustup update nightly

$ rustup doc
$ rustup man
```
