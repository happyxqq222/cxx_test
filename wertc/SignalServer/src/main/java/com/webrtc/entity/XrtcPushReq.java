package com.webrtc.entity;

import lombok.Data;

@Data
public class XrtcPushReq {
    int cmdNo;
    long uid;
    String streamName;
    int audio;
    int video;

}
