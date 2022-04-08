FROM debian:bullseye

RUN apt-get update && apt-get install -y \
    # build-essential \
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

# Create profiles
# .. GCC
RUN conan profile new gcc --detect \
    && conan profile update settings.compiler.libcxx=libstdc++11 gcc

# .. clang
RUN conan profile new clang --detect \
    && conan profile update settings.compiler=clang clang \
    && conan profile update settings.compiler.version=11 clang \
    && conan profile update settings.compiler.libcxx=libstdc++11 clang \