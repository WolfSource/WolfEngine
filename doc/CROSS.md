# Cross Build

```shell
# 1st WAY
$ make release CARGO="cross" PACKAGE="fip_api" RELEASE="--release" STRIP="aarch64-linux-gnu-strip" TARGET="aarch64-unknown-linux-musl" VERSION="$(git describe --abbrev=0 --tags)"

# run before run the rest
$ docker build . --file ./.github/actions/package/Dockerfile --tag rust-build:1.56.1-bullseye

# 2nd WAY
$ docker run --volume $(pwd):/workspace --volume /var/run/docker.sock:/var/run/docker.sock --workdir /workspace --rm rust-build:1.56.1-bullseye make release CARGO="cross" PACKAGE="fip_api" RELEASE="--release" STRIP="aarch64-linux-gnu-strip" TARGET="aarch64-unknown-linux-musl" VERSION="$(git describe --abbrev=0 --tags)"

# 2nd WAY: devcontainer docker-from-docker
$ docker run --volume ${LOCAL_WORKSPACE_FOLDER}:/workspace --workdir /workspace --rm rust-build:1.56.1-bullseye make release CARGO="cross" PACKAGE="fip_api" RELEASE="--release" STRIP="aarch64-linux-gnu-strip" TARGET="aarch64-unknown-linux-musl" VERSION="$(git describe --abbrev=0 --tags)"
```
