# pipesocks
A pipe-like SOCKS5 tunnel system. 

## Introduction
Pipesocks is aimed at going through firewalls which are built by national force with strong encryption and relay servers to avoid content supervision. 

This software consists of 4 parts: pump, pipe, tap, and PAC. Pump is the server outside the firewall to provide information for tap. Pipe is a relay server which unconditionally transfers data through pump and tap. Tap is a client running in local with SOCKS5 interface for native clients and connects to pump or pipe. PAC is an HTTP server providing proxy auto configuration files for mobile operating system such as iOS and Android. 

A pipe or a tap is valid only if: 

1) It is directly connected to the pump. 

Or

2) It is connected to a valid pipe. 

###### Note that this program currently only supports TCP but UDP. 

## Deployment
### General
Download & install Qt Creator with Qt (above 5) and libsodium. Open the project with Qt Creator, set include path and link path and compile. Then, copy the file "proxy.pac" to the binary directory or create "proxy.pac" yourself if you are an expert:) (You don't have to do it under Mac OS X because pipesocks does not support the function under this operating system)

### Ubuntu Server
Since most Ubuntu servers doesn't hold a GUI, here is the way to deploy pipesocks in them. 

Type these commands in your terminal: 

```bash
sudo apt-get -y install git make build-essential qt5-default qt5-qmake
git clone https://github.com/jedisct1/libsodium.git
git clone https://github.com/yvbbrjdr/pipesocks.git
cd libsodium/
git checkout stable
./configure
make && sudo make install
sudo cp /usr/local/lib/libsodium.so.18 /usr/lib/
cd ../pipesocks/pipesocks/
qmake && make
./pipesocks pump
```

And you'll start a pipesocks pump server. 

### OS X
To install libsodium, you can type these commands in your terminal: 

```bash
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null
brew install libsodium
```

### Windows
If you don't want to compile libsodium yourself, you can download the precompiled library right [here](https://download.libsodium.org/libsodium/releases/). 

## Usage
### Graphical User Interface
You can start pipesocks GUI by double-clicking the program icon or running it in the terminal providing no arguments. 

### Command-Line Interface
Following is the command to be typed to the terminal. 
```
{Binary Directory}/pipesocks <pump|pipe|tap|pac> [-H Remote Host] [-P Remote Port] [-p Local Port] [-k Password]
```

## Dependencies
[Qt](http://www.qt.io/)

[libsodium](https://download.libsodium.org/doc/)
