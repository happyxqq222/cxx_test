'use strict';

let pushBtn = document.getElementById("pushBtn");
pushBtn.addEventListener("click",startPush);

let uid = $("#uid").val();
let streamName = $("#streamName").val();
let audio = $("#audio").val();
let video = $("#video").val();


function startPush() {
    console.log("send push: /signaling/push");
    $.post({
        url:"/signaling/push",
        contentType:"application/json",
        data:JSON.stringify({
            "uid":uid,
            "streamName":streamName,
            "audio":audio,
            "video":video
        }),
        success:requsetSuccess,
        error:requestError
    });
}

function requsetSuccess(response){
    console.log("success:"+response);
}

function requestError(xhr,status,error){
    console.log("error status:"+status + ", error:" + error);
}