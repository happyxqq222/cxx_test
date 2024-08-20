package com.webrtc.entity;

import lombok.Data;

@Data
public class XrtcPushResp {
    int errno;
    int errMsg;
    String offer;
}
