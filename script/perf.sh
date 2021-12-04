#!/bin/bash

set -eux
set -o errexit
set -o pipefail
set -o nounset

# STEP 0: Make sure there is no left-over profiling data from previous runs
rm -fr /tmp/pgo-data

# STEP 1: Build the instrumented binaries
RUSTFLAGS="-Cprofile-generate=/tmp/pgo-data" \
    cargo build --package fip_api --release --target x86_64-apple-darwin

# STEP 2: Run the instrumented binaries with some typical data
../target/x86_64-apple-darwin/release/myprogram-mydata1.csv
../target/x86_64-apple-darwin/release/myprogram-mydata2.csv
../target/x86_64-apple-darwin/release/myprogram-mydata3.csv

# STEP 3: Merge the `.profraw` files into a `.profdata` file
llvm-profdata merge --output=/tmp/pgo-data/merged.profdata /tmp/pgo-data

# STEP 4: Use the `.profdata` file for guiding optimizations
RUSTFLAGS="-Cllvm-args=-pgo-warn-missing-function -Cprofile-use=/tmp/pgo-data/merged.profdata" \
    cargo build --package fip_api --release --target x86_64-apple-darwins
