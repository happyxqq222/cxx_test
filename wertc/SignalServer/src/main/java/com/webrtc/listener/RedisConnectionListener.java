package com.webrtc.listener;

import lombok.extern.slf4j.Slf4j;
import org.redisson.api.NodeType;
import org.redisson.connection.ConnectionListener;

import java.net.InetSocketAddress;

@Slf4j
public class RedisConnectionListener implements ConnectionListener {

    @Override
    public void onConnect(InetSocketAddress addr) {

    }

    @Override
    public void onDisconnect(InetSocketAddress addr) {

    }

    @Override
    public void onConnect(InetSocketAddress addr, NodeType nodeType) {
        log.info("onConnect {}:{} ,nodeType:{}", addr.getHostName(), addr.getPort(), nodeType.name());
    }

    @Override
    public void onDisconnect(InetSocketAddress addr, NodeType nodeType) {
        log.info("onDisconnect {}:{}, nodeType:{}", addr.getHostName(), addr.getPort(),nodeType.name());
    }
}
