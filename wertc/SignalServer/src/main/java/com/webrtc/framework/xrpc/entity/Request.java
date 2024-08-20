package com.webrtc.framework.xrpc.entity;

import lombok.Data;

@Data
public class Request {
    private RequestHeader requestHeader;
    private String requestBody;

    public Request(String body, int logId){
        RequestHeader requestHeader = new RequestHeader();
        requestHeader.setLogid(logId);
        requestHeader.setMagicNum(RequestHeader.HEADER_MAGICNUM);
        requestHeader.setBodyLen(body.length());
    }
}
