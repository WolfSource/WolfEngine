# syntax=docker.io/docker/dockerfile:1.3.1

ARG RUST_VERSION="1.57.0-bullseye"
FROM rust:${RUST_VERSION}
RUN set -eux \
    && export DEBIAN_FRONTEND="noninteractive" \
    && apt-get update \
    && apt-get install --no-install-recommends --yes \
        binutils-aarch64-linux-gnu \
        binutils-arm-linux-gnueabihf \
        binutils-x86-64-linux-gnu \
        docker.io \
        musl-tools \
    && apt-get autoremove --yes \
    && apt-get clean --yes \
    && rm -fr /tmp/* /var/lib/apt/lists/* /var/tmp/*
ENV CROSS_DOCKER_IN_DOCKER=true
RUN cargo install --locked cross
