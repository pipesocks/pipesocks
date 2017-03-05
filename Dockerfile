FROM debian:stable-slim
ENV version=2.3 \
    port=7473 \
    password=""
ADD https://github.com/pipesocks/pipesocks/releases/download/$version/pipesocks-$version-linux.tar.xz pipesocks.tar.xz
RUN apt-get update -y && \
    apt-get install xz-utils && \
    tar -xJf pipesocks.tar.xz && \
    apt-get remove xz-utils && \
    apt-get autoremove
WORKDIR pipesocks/
EXPOSE $port
CMD ./runpipesocks.sh pump -p $port -k $password
