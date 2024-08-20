package com.webrtc.framework.xrpc.net;

import com.webrtc.framework.xrpc.entity.RequestHeader;
import com.webrtc.framework.xrpc.entity.Response;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.LengthFieldBasedFrameDecoder;
import io.netty.util.CharsetUtil;

import java.util.concurrent.CountDownLatch;

public class XRpcFrameDecoder extends LengthFieldBasedFrameDecoder {


    public XRpcFrameDecoder( Response response) {
        super(8192,32,4,0,36);
    }

    @Override
    protected Object decode(ChannelHandlerContext ctx, ByteBuf in) throws Exception {
        if(in.readableBytes() < RequestHeader.HEADER_SIZE) {
            return null;
        }
        in.markReaderIndex();
        /*
            private short id;
    private short version;
    private int logid;
    private char[] provider;
    private int magicName;
    private int reserved;
    private int bodyLen;
         */
        RequestHeader requestHeader = new RequestHeader();
        requestHeader.setId(in.readShort());
        requestHeader.setVersion(in.readShort());
        requestHeader.setLogid(in.readInt());
        requestHeader.setProvider(in.readBytes(16).array());
        requestHeader.setMagicNum(in.readInt());
        requestHeader.setReserved(in.readInt());
        requestHeader.setBodyLen(in.readInt());
        if(in.readableBytes() < requestHeader.getBodyLen()) {
            in.resetReaderIndex();
            return null;
        }
        byte[] body = new byte[requestHeader.getBodyLen()];
        in.readBytes(body);
        String bodyStr = new String(body, CharsetUtil.UTF_8);


        //读
        return super.decode(ctx, in);
    }
}
