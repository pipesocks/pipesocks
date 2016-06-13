# pipesocks
A pipe-like SOCKS5 tunnel system. 

## Introduction

Pipesocks is aimed at going through firewalls which are built by national force with strong encryption and relay servers to avoid content supervision. 

This software consists of 4 parts: pump, pipe, tap, and PAC. Pump is the server outside the firewall to provide information for tap. Pipe is a relay server which unconditionally transfers data through pump and tap. Tap is a client running in local with SOCKS5 interface connecting to pump or pipe. PAC is an http server providing proxy auto configuration files for mobile operating system such as iOS and Android. 
