package com.webrtc.framework.xrpc.entity;

import lombok.Data;

@Data
public class Client {
    private String server;
    private int port;
    private int connectionTimeout;
    private int readTimeout;
    private int writeTimeout;
}
