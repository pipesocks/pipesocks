FROM debian:stable-slim
ENV version=2.3 \
    port=7473 \
    password=""
RUN apt-get update -y && \
    apt-get install -y curl xz-utils libglib2.0-0 && \
    curl -SL https://github.com/pipesocks/pipesocks/releases/download/$version/pipesocks-$version-linux.tar.xz | \
    tar -xJ && \
    apt-get remove -y curl xz-utils && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*
WORKDIR pipesocks/
EXPOSE $port
CMD ./runpipesocks.sh pump -p $port -k $password
