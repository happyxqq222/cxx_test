package com.webrtc.controller;


import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class SignalController {

    @GetMapping("/xrtcclient/push")
    public String xrtcClientPush(Model model){
        model.addAttribute("uid","11");
        model.addAttribute("streamName","aadsfsd");
        model.addAttribute("audio","ahhh");
        model.addAttribute("video","vvv");
        return "push";
    }
}
