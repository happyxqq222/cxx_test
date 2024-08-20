package com.webrtc.framework.xrpc.net;

import com.webrtc.framework.xrpc.entity.Client;
import com.webrtc.framework.xrpc.entity.Request;
import com.webrtc.framework.xrpc.entity.Response;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.handler.timeout.WriteTimeoutException;
import io.netty.handler.timeout.WriteTimeoutHandler;
import lombok.extern.slf4j.Slf4j;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;


@Slf4j
public class XrpcClient {

    private CountDownLatch writeLatch = new CountDownLatch(1);
    private CountDownLatch readLatch = new CountDownLatch(1);

    private volatile Throwable writeException = null;

    public XrpcClient() {
    }

    public Response request(Client client, Request request) throws Exception {
        NioEventLoopGroup group = new NioEventLoopGroup(1);

        try {

            Response response = new Response();
            Bootstrap bootstrap = new Bootstrap();
            bootstrap.group(group);
            bootstrap.option(ChannelOption.CONNECT_TIMEOUT_MILLIS, client.getConnectionTimeout());
            bootstrap.handler(new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel socketChannel) throws Exception {
                    socketChannel.pipeline().addLast(new LoggingHandler(LogLevel.INFO));
                    socketChannel.pipeline().addLast(new WriteTimeoutHandler(client.getWriteTimeout(), TimeUnit.MILLISECONDS));
                    socketChannel.pipeline().addLast(new XRpcFrameEncodr(XrpcClient.this));
                    socketChannel.pipeline().addLast(new XRpcFrameDecoder(response));
                }
            });
            ChannelFuture future = bootstrap.connect(client.getServer(), client.getPort()).sync();
            Channel channel = future.channel();
            channel.writeAndFlush(request).addListener((ChannelFutureListener) f -> {
                if (!f.isSuccess() && f.cause() instanceof WriteTimeoutException) {
                    notifyMainThread(f.cause());
                }
            });
            writeLatch.await();
            if (writeException != null) {
                if (writeException instanceof WriteTimeoutException) {
                    throw (WriteTimeoutException) writeException;
                } else {
                    throw new RuntimeException(writeException.getCause());
                }
            }
            boolean ret = false;
            ret = readLatch.await(client.getReadTimeout(), TimeUnit.MILLISECONDS);
            
        } catch (Exception e) {
            throw e;
        } finally {
            group.shutdownGracefully();
        }

        return null;
    }

    public void notifyMainThread(Throwable cause) {
        writeException = cause;
        writeLatch.countDown();
    }


    public void sendMessage(Channel channel, Request request) throws InterruptedException {
        ChannelFuture channelFuture = channel.writeAndFlush(request);
    }


}
