FROM debian:bullseye

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    ninja-build \
    git \
    # libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

