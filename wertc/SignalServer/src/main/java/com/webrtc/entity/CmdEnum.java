package com.webrtc.entity;

public enum CmdEnum {
    CMDNO_PUSH(1),
    CMDNO_PULL(2),
    CMDNO_ANSWER(3),
    CMDNO_STOP_PUSH(4),
    CMDNO_STOP_PULL(5);

    CmdEnum(int cmdCode){
        this.cmdCode = cmdCode;
    }

    public int cmdNo(){
        return cmdCode;
    }

    private final int cmdCode;

}
