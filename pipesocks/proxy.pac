function FindProxyForURL(url,host) {
    var server="127.0.0.1:1080";
    return "SOCKS5 "+server+";SOCKS "+server+";DIRECT";
}
