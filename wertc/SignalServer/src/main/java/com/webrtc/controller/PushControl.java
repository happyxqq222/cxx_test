package com.webrtc.controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.webrtc.entity.CmdEnum;
import com.webrtc.entity.PushEntity;
import com.webrtc.entity.XrtcPushReq;
import com.webrtc.entity.XrtcPushResp;
import com.webrtc.framework.XrtcRequestService;
import jakarta.annotation.Resource;
import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@Slf4j
@RestController
public class PushControl {

    @Resource
    private XrtcRequestService xrtcRequestService;

    @PostMapping("/signaling/push")
    public String push(@RequestBody PushEntity pushEntity) throws JsonProcessingException {
        ObjectMapper mapper = new ObjectMapper();
        String json = mapper.writeValueAsString(pushEntity);
        log.info("receive push json:{}",json);
        XrtcPushReq req = new XrtcPushReq();
        req.setAudio(pushEntity.getAudio());
        req.setVideo(pushEntity.getVideo());
        req.setUid(pushEntity.getUid());
        req.setStreamName(pushEntity.getStreamName());
        req.setCmdNo(CmdEnum.CMDNO_PUSH.cmdNo());

        XrtcPushResp resp = xrtcRequestService.call("192.168.17.128");

        return "1";
    }
}
