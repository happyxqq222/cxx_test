package com.webrtc.framework.xrpc.entity;

import lombok.Data;

@Data
public class RequestHeader {
    public static int HEADER_SIZE = 36;
    public static int HEADER_MAGICNUM = 0xfb202202;

    private short id;
    private short version;
    private int logid;
    private byte[] provider;
    private int magicNum;
    private int reserved;
    private int bodyLen;
}
