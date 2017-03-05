FROM debian:stable-slim
ENV version=2.3 \
    port=7473 \
    password=""
COPY https://github.com/pipesocks/pipesocks/releases/download/$version/pipesocks-$version-linux.tar.xz pipesocks.tar.xz
RUN tar -xJf pipesocks.tar.xz
WORKDIR pipesocks/
EXPOSE $port
CMD ./runpipesocks.sh pump -p $port -k $password
