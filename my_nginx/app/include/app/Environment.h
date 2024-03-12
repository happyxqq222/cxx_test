//
// Created by Administrator on 2024-03-11.
//

#ifndef MYNGINX_ENVIRONMENT_H
#define MYNGINX_ENVIRONMENT_H

#include <vector>
#include <string>

class Environment {
public:
    static Environment* getInstance(int argc, char* argv[]);
private:
    Environment(int argc, char* argv[]);
    ~Environment();
private:
    std::vector<std::string*> environemnts;
    std::vector<std::string*> programArgs;
};


#endif //MYNGINX_ENVIRONMENT_H
