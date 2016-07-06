# pipesocks
A pipe-like SOCKS5 tunnel system. 

## Introduction
Pipesocks is aimed at going through firewalls which are built by national force with strong encryption and relay servers to avoid content supervision. 

This software consists of 4 parts: pump, pipe, tap, and PAC. Pump is the server outside the firewall to provide information for tap. Pipe is a relay server which unconditionally transfers data through pump and tap. Tap is a client running in local with SOCKS5 interface for native clients and connects to pump or pipe. PAC is an HTTP server providing proxy auto configuration files for mobile operating system such as iOS and Android. 

A pipe or a tap is valid only if: 

1) It is directly connected to the pump. 

Or

2) It is connected to a valid pipe. 

Note that this program currently only supports TCP but UDP. 

## Dependencies
Qt

libsodium