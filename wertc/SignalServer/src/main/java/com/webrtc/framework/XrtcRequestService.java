package com.webrtc.framework;

import com.webrtc.config.RpcConfig;
import com.webrtc.entity.XrtcPushResp;
import com.webrtc.framework.xrpc.entity.Client;
import jakarta.annotation.PostConstruct;
import jakarta.annotation.Resource;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
@Slf4j
public class XrtcRequestService {

    @Resource
    private RpcConfig rpcConfig;

    private Map<String, Client> clients = new HashMap<>();

    @PostConstruct
    public void initXrpc(){
        List<RpcConfig.RpcInfo> rpcInfos = rpcConfig.getRpcInfos();
        for (RpcConfig.RpcInfo rpcInfo : rpcInfos) {
            Client client = new Client();
            client.setServer(rpcInfo.getServer());
            client.setPort(rpcInfo.getPort());
            client.setConnectionTimeout(rpcInfo.getConnectTimeout());
            client.setReadTimeout(rpcInfo.getReadTimeout());
            client.setWriteTimeout(rpcInfo.getWriteTimeout());
            clients.put(client.getServer(),client);
        }
    }

    public XrtcPushResp call(String serviceName){
        Client client = clients.get(serviceName);
        if(client == null){
            log.error("no service name:{}",serviceName);
            throw new RuntimeException("no service name");
        }
        log.info("client :{}",client.toString());
        return null;
    }
}
