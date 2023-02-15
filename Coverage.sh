#!/bin/bash

rustup default nightly
cargo install grcov
rustup component add llvm-tools-preview
cargo clean
cargo clippy --all-targets --frozen --message-format human --workspace -- --deny clippy::all --deny clippy::cargo --deny clippy::nursery --deny clippy::pedantic --deny warnings --allow clippy::multiple_crate_versions --allow clippy::module_name_repetitions
RUSTFLAGS='-Zinstrument-coverage' LLVM_PROFILE_FILE='wolf-coverage-%p-%m.profraw' cargo test
grcov . -s . --ignore ./wolf/build.rs --ignore ./wolf/sys/* --ignore ./wolf/src/system/ffi/* --binary-path ./target/debug/ -t html --branch --ignore-not-existing -o ./coverage/