package com.webrtc.framework.xrpc.net;

import com.webrtc.framework.xrpc.entity.Request;
import com.webrtc.framework.xrpc.entity.RequestHeader;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelOutboundHandlerAdapter;
import io.netty.channel.ChannelPromise;
import io.netty.handler.codec.MessageToByteEncoder;
import io.netty.handler.timeout.WriteTimeoutException;
import io.netty.util.CharsetUtil;

import java.io.IOException;

@ChannelHandler.Sharable
public class XRpcFrameEncodr extends ChannelOutboundHandlerAdapter {

    private final XrpcClient xrpcClient;

    public XRpcFrameEncodr(XrpcClient xrpcClient) {
        this.xrpcClient = xrpcClient;
    }


    @Override
    public void write(ChannelHandlerContext ctx, Object msg, ChannelPromise promise) throws Exception {
        Request request = (Request) msg;
        ByteBuf out = ctx.alloc().buffer();
        RequestHeader requestHeader = request.getRequestHeader();
        out.writeShort(requestHeader.getId());
        out.writeShort(requestHeader.getVersion());
        out.writeInt(requestHeader.getLogid());
        out.writeBytes(requestHeader.getProvider());
        out.writeInt(requestHeader.getMagicNum());
        out.writeInt(requestHeader.getReserved());
        int bodyLen = request.getRequestBody().length();
        out.writeInt(bodyLen);
        out.writeBytes(request.getRequestBody().getBytes(CharsetUtil.UTF_8));
        ctx.writeAndFlush(out,promise);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        if(cause instanceof WriteTimeoutException){
            xrpcClient.notifyMainThread(cause);
        }
    }
}
