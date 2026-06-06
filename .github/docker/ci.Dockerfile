FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++-14 \
    make \
    valgrind \
    libgtest-dev \
    cmake \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Compile et installe Google Test (libgtest-dev fournit seulement les sources)
RUN cd /usr/src/googletest && \
    cmake -DBUILD_SHARED_LIBS=OFF . && \
    make -j"$(nproc)" && \
    make install && \
    cd / && rm -rf /usr/src/googletest

# Définit g++-14 comme compilateur par défaut
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100
