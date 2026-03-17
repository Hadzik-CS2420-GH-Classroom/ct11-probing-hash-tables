FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# Install extra tools useful for class
RUN apt-get update && apt-get install -y \
    valgrind \
        gdb \
            make \
                cmake \
                    && apt-get clean \
                        && rm -rf /var/lib/apt/lists/*