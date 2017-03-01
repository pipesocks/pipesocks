FROM ubuntu:latest
ADD https://github.com/pipesocks/pipesocks/releases/download/2.3/pipesocks-2.3-linux.tar.xz pipesocks-2.3-linux.tar.xz
RUN apt-get update -y && \
    apt-get install -y xz-utils libglib2.0-0 && \
    tar -xJf pipesocks-2.3-linux.tar.xz
WORKDIR pipesocks/
CMD ./runpipesocks.sh pump
