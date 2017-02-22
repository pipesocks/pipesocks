<img src="https://raw.githubusercontent.com/pipesocks/pipesocks/stable/pipesocks/icons/origin.png" width="150" height="150" alt="pipesocks" />

# pipesocks

[![pipesocks](https://img.shields.io/badge/pipesocks-2.2-yellow.svg?style=flat)](https://github.com/pipesocks/pipesocks)
[![yvbbrjdr](https://img.shields.io/badge/Coded%20by-yvbbrjdr-00ffff.svg?style=flat)](https://github.com/yvbbrjdr)
[![powered](https://img.shields.io/badge/Powered%20by-Qt%20%7C%20libsodium-green.svg?style=flat)](https://github.com/pipesocks/pipesocks#dependencies)
[![platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-ff69b4.svg?style=flat)](http://doc.qt.io/qt-5/supported-platforms.html)
[![license](https://img.shields.io/badge/LICENSE-GPL%20v3-blue.svg?style=flat)](http://www.gnu.org/licenses/gpl-3.0.html)

A pipe-like SOCKS5 tunnel system. 

## Introduction
Pipesocks is aimed at going through firewalls which are built by national force, with strong encryption and relay servers to avoid content censorship. 

This software consists of 3 parts: pump, pipe, and tap. Pump is the server outside the firewall to provide Internet data for tap. Pipe is a relay server which unconditionally transfers data through pump and tap. Tap is a client running in local with SOCKS5 interface for native clients and connects to pump or pipe. Tap can also provide PAC script if you request it using HTTP GET. You can also get GFWList PAC script if you include "gfwlist" in the HTTP packet.

A pipe or a tap is valid only if: 

1) It is directly connected to the pump. 

Or

2) It is connected to a valid pipe. 

###### This program supports TCP and UDP.

## Tutorial

A full tutorial in Mandarin about how to use pipesocks is located [here](https://pipesocks.github.io/md/tutorial).

## Download

Pre-compiled files for various platforms can be found [here](https://pipesocks.github.io).

## Compilation
### General
Download & install Qt Creator with Qt (above 5.7) and libsodium. Open the project with Qt Creator, set include path and link path and compile. 

### Ubuntu Server
Since most Ubuntu servers don't hold a GUI, here is the way to deploy pipesocks in them. 

Type this command in your terminal: 

```bash
curl https://raw.githubusercontent.com/pipesocks/pipesocks/stable/install.sh | sh
```

And you'll get a pipesocks binary file. 

### macOS
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
