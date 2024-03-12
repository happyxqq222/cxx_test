//
// Created by Administrator on 2024-03-12.
//

#ifndef MYNGINX_NGINXPROCESS_H
#define MYNGINX_NGINXPROCESS_H
#include <string_view>


class NginxProcess {
private:
    NginxProcess();
    void masterProcessCycle();
    void workerProcessCycle();
    void startWorkerProcess(int);
    int spawnProcess(int,std::string_view title);
public:
    static NginxProcess* getInstance();

};


#endif //MYNGINX_NGINXPROCESS_H
