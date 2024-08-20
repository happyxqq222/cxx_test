package com.webrtc.entity;

import lombok.Data;

@Data
public class PushEntity {

    private long uid;
    private String streamName;
    private int audio;
    private int video;
}
