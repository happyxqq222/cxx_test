package com.webrtc.controller;

import com.webrtc.entity.PushEntity;
import jakarta.annotation.Resource;
import lombok.extern.slf4j.Slf4j;
import org.redisson.api.RLock;
import org.redisson.api.RedissonClient;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.concurrent.TimeUnit;

@Slf4j
@RestController("")
public class HelloControl {

    @Resource
    RedisTemplate<String,Object> redisTemplate;

    @Resource
    RedissonClient redissonClient;

    static int count = 0;

    @GetMapping("/")
    String hello(){
        PushEntity pushEntity = new PushEntity();
        pushEntity.setAudio(1);
        pushEntity.setUid(11233);
        pushEntity.setVideo(1);
        redisTemplate.opsForValue().set("ab",pushEntity);
        return "hello";
    }

    @GetMapping("/h2")
    String h2(){
        RLock lock = redissonClient.getLock("aaaf");
        try{
            lock.lock();
            System.out.println(++count);
            TimeUnit.SECONDS.sleep(5);
        }catch (InterruptedException e){
            e.printStackTrace();
        } finally {
            lock.unlock();
        }
        return "h2";
    }
}
