package com.webrtc.config;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

import java.util.List;

@Data
@Component
@ConfigurationProperties(prefix = "spring.data.redis.cluster")
public class RedisClusterConfig {

    List<String> nodes ;
}
