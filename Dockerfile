FROM ubuntu:latest
RUN apt-get update -y && \
    apt-get install -y xz-utils libglib2.0-0
ADD https://github.com/pipesocks/pipesocks/releases/download/2.3/pipesocks-2.3-linux.tar.xz pipesocks-2.3-linux.tar.xz
RUN tar -xJf pipesocks-2.3-linux.tar.xz
WORKDIR pipesocks/
EXPOSE 7473
CMD ./runpipesocks.sh pump
