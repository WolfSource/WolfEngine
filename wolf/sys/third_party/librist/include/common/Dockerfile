FROM buildpack-deps:focal as builder

# Get build tools and dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
        python3-pip \
        cmake \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install meson ninja

RUN git clone https://code.videolan.org/rist/librist.git && \
    cd librist && \
    mkdir build && \
    cd build && \
    meson .. --default-library=static --buildtype=release -Db_lto=true && \
    ninja

FROM ubuntu:focal as release

COPY --from=builder /librist/build/tools/rist2rist /usr/bin/
COPY --from=builder /librist/build/tools/ristreceiver /usr/bin/
COPY --from=builder /librist/build/tools/ristsender /usr/bin/
COPY --from=builder /librist/build/tools/ristsrppasswd /usr/bin/
