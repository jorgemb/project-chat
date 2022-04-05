# BUILD
FROM debian:bullseye as Build

# Install components
RUN apt-get update && apt-get install -y\
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

COPY . /src/

WORKDIR /build/
RUN cmake /src/ && cmake --build . -j4

# TARGET
FROM debian:bullseye
WORKDIR /app/
COPY --from=Build /build/chat_* .