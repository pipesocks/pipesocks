<img src="https://raw.githubusercontent.com/yvbbrjdr/pipesocks/stable/pipesocks/icons/origin.png" width="150" height="150" alt="pipesocks" />

# pipesocks
A pipe-like SOCKS5 tunnel system. 

## Introduction
Pipesocks is aimed at going through firewalls which are built by national force, with strong encryption and relay servers to avoid content supervision. 

This software consists of 3 parts: pump, pipe, and tap. Pump is the server outside the firewall to provide Internet data for tap. Pipe is a relay server which unconditionally transfers data through pump and tap. Tap is a client running in local with SOCKS5 interface for native clients and connects to pump or pipe. Tap can also provide PAC script if you request it using HTTP GET. 

A pipe or a tap is valid only if: 

1) It is directly connected to the pump. 

Or

2) It is connected to a valid pipe. 

###### Note that this program only supports TCP. 

## Deployment
### General
Download & install Qt Creator with Qt (above 5) and libsodium. Open the project with Qt Creator, set include path and link path and compile. 

### Ubuntu Server
Since most Ubuntu servers doesn't hold a GUI, here is the way to deploy pipesocks in them. 

Type this command in your terminal: 

```bash
curl https://raw.githubusercontent.com/yvbbrjdr/pipesocks/stable/install.sh | sh
```

And you'll get a pipesocks binary file. 

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
You can start pipesocks GUI by double-clicking the program icon or running it in the terminal, providing no arguments. 

### Command-Line Interface
Following are the commands to be typed in the terminal. 
```
./pipesocks pump [-p Local Port] [-k Password]
./pipesocks pipe <-H Remote Host> [-P Remote Port] [-p Local Port]
./pipesocks tap <-H Remote Host> [-P Remote Port] [-p Local Port] [-k Password]
```

## Dependencies
[Qt](http://www.qt.io/)

[libsodium](https://download.libsodium.org/doc/)
