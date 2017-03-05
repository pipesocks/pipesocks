FROM debian:stable-slim
ENV version=2.3 \
    port=7473 \
    password=""
ADD https://github.com/pipesocks/pipesocks/releases/download/$version/pipesocks-$version-linux.tar.xz pipesocks.tar.xz
RUN apt-get update -y && \
    apt-get install -y xz-utils libglib2.0-0 && \
    tar -xJf pipesocks.tar.xz && \
    rm pipesocks.tar.xz && \
    apt-get remove -y xz-utils && \
    apt-get autoremove -y
WORKDIR pipesocks/
EXPOSE $port
CMD ./runpipesocks.sh pump -p $port -k $password
