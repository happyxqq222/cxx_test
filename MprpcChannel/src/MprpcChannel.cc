#include "MprpcChannel.h"

using namespace google::protobuf;
using namespace std;
void CallMethod(const MethodDescriptor *method,
                RpcController *controller,
                const Message *request,
                Message *response,
                Closure *done) {}