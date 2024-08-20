#!/usr/bin/bash
cd example/
protoc user.proto --cpp_out=./

cd ../src
protoc RpcHeader.proto --cpp_out=./