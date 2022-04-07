FROM debian:bullseye

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    # clangd \
    cmake \
    ninja-build \
    git \
    # libboost-all-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install conan
RUN pip install conan


