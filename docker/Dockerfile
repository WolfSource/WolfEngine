ARG UBUNTU_VERSION="22.04"
FROM ubuntu:${UBUNTU_VERSION}

ENV VCPKG_ROOT=/opt/vcpkg
ARG VCPKG_GITHUB_REPOSITORY=https://github.com/microsoft/vcpkg/archive/master.tar.gz
ARG VCPKG_COMPRESSED_FILE_NAME=vcpkg.tar.gz

RUN set -eux \
	&& export DEBIAN_FRONTEND="noninteractive" \
	&& apt-get update \
	&& apt-get install --yes --no-install-recommends \
		build-essential \
		ca-certificates \
		cmake \
		cpp \
		curl \
		git \
		make \ls
		pkg-config \
		tar \
		unzip \
		wget \
		zip

WORKDIR  ${VCPKG_ROOT}

RUN wget -qO ${VCPKG_COMPRESSED_FILE_NAME} ${VCPKG_GITHUB_REPOSITORY} \
	&& tar xf ${VCPKG_COMPRESSED_FILE_NAME} --strip-components=1 \
	&& rm ${VCPKG_COMPRESSED_FILE_NAME} \
	&& /opt/vcpkg/bootstrap-vcpkg.sh \
	&& ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg

WORKDIR /workspace

COPY . .

RUN cmake -B build --preset linux-x64-release

RUN rm -rf ./*
