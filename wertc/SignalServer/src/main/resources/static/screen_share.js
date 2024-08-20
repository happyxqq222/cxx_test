'use strict';

const localVideo = document.getElementById('localVideo');

const startPushBtn = document.getElementById("btnStartPush");
startPushBtn.addEventListener('click',startPush);
const startPullBtn = document.getElementById("btnStartPull");
startPullBtn.addEventListener('click',startPull);

const config = {};

const offerOptions = {
    offerToReceiveAudio: false,
    offerToReceiveVideo: false
}

let pc1 = new RTCPeerConnection(config);
let pc2 = new RTCPeerConnection(config);

function startPush() {
    console.log("startPush stream");
    window.postMessage({type: 'SS_UI_REQUEST', text: "push"}, '*');
}

function startPull() {
    console.log("startPull stream");
    pc2.createAnswer().then(
        onCreateAnswerSuccess,
        onCreateSessionDescriptionError
    );
}

window.addEventListener('message',function(event){
    if(event.origin != window.location.origin) {
        return;
    }
    if(event.data.type && event.data.type === 'SS_DIALOG_SUCCESS') {
        console.log("用户同意屏幕共享, streamId:" + event.data.streamId);
        startScreenStreamFrom(event.data.streamId);
    } else if(event.data.type && event.data.type === "SS_DIALOG_CANCEL") {
        console.log("用户取消屏幕共享");
    }
});

function startScreenStreamFrom(streamId) {
    const constraints = {
        audio: false,
        video:{
            mandatory:{
                chromeMediaSource: 'desktop',
                chromeMediaSourceId: streamId,
                maxWidth: window.screen.width,
                maxHeight:window.screen.height
            }
        }
    };
    navigator.mediaDevices.getUserMedia(constraints).then(handleSuccess).catch(handleError);
}

function handleSuccess(stream) {
    console.log("get screen stream success");
    localVideo.srcObject = stream;
    pc1.onconnectionstatechange = function (e) {
        onIceStateChange(pc1,e);
    };
    pc1.onicecandidate = function (e) {
        onIceCandiate(pc1,e);
    }
    pc1.addStream(stream);

    pc1.createOffer(offerOptions).then(
        onCreateOfferSuccess,
        onCreateSessionDescriptionError
    )
}

function getPc(pc) {
    return pc == pc1 ? 'pc1' : 'pc2';
}

function onIceStateChange(pc, e) {
    console.log(getPc(pc));
    console.log(getPc(pc) + ' ice state change: ' + pc.iceConnectionState);
}

function onIceCandiate(pc,e) {
    console.log(getPc(pc) + ' get new ice candiate: ' + (e.candidate? e.candidate.candidate : '(null)'));
}

function onCreateOfferSuccess(desc) {
    console.log('offer from pc1:\n' + desc.sdp);
    console.log('pc1 set local description start');
    pc1.setLocalDescription(desc).then(
        function () {
            onSetLocalSuccess(pc1);
        },
        onSetSessionDescriptionError
    )
    //sdp交换
    pc2.oniceconnectionstatechange = function (e) {
        onIceStateChange(pc2,e);
    }
    pc2.onicecandidate = function (e) {
        onIceCandiate(pc2,e);
    }
    pc2.onaddstream = function (e) {
        console.log('pc2 receive stream, stream_id: ' + e.stream.id);
    }
    pc2.setRemoteDescription(desc).then (
        function() {
            onSetRemoteSuccess(pc2);
        },
        onSetSessionDescriptionError
    );
}

function onCreateAnswerSuccess(desc) {
    console.log("answer from pc2: \n" + desc.sdp);
    console.log("pc2 set local description start");
    pc2.setLocalDescription(desc).then(
        function () {
            onSetLocalSuccess(pc2);
        },
        onSetSessionDescriptionError
    );
    //交换sdp
    pc1.setRemoteDescription(desc).then(
        function () {
            onSetRemoteSuccess(pc1);
        },
        onSetSessionDescriptionError
    )
}

function onCreateSessionDescriptionError(error) {
    console.log('create session description error: ' + error.toString());
}

function onSetLocalSuccess(pc){
    console.log("onSetLocalSuccess " + getPc(pc));
}

function onSetRemoteSuccess(pc) {
    console.log(getPc(pc) +" onSetRemoteSuccess");
}


function onSetSessionDescriptionError(error) {
    console.log('SetSessionDescriptionError :' + error.toString());
}

function handleError(err) {
    console.log("get screen stream error");
}

