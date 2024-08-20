package com.webrtc.config;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import java.util.List;

@Configuration
@ConfigurationProperties(prefix = "server")
@Data
public class RpcConfig {

    private List<RpcInfo> rpcInfos;

    @Data
    public static class RpcInfo {
        private String server;
        private int port;
        private int connectTimeout;
        private int readTimeout;
        private int writeTimeout;
    }

}
