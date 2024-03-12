//
// Created by Administrator on 2024-03-08.
//

#ifndef MYNGINX_NGX_C_CONF_H
#define MYNGINX_NGX_C_CONF_H

#include <vector>
#include <mutex>
#include <string_view>
#include <optional>
#include "ngx_global.h"

class CConfig
{
private:
    CConfig();
    static std::mutex m;
public:
    ~CConfig();
private:
    static CConfig *m_instance;
public:
    static CConfig* getInstance(){
        if(m_instance == nullptr){
            std::unique_lock<std::mutex> lock;
            if(m_instance == nullptr){
                m_instance = new CConfig;
                static CGarbageCollection cl;
            }
        }
        return m_instance;
    }
    class CGarbageCollection{
    public:
        ~CGarbageCollection(){
            delete CConfig::m_instance;
            CConfig::m_instance = nullptr;
        }
    };
public:
    bool load(std::string_view pConfName);
    const std::optional<std::string> getString(std::string_view pItemName);
    std::optional<int> getIntDefault(std::string_view  pItemName, const int def);
private:
    std::vector<LPCConfItem> mConfigItemList;
};


#endif //MYNGINX_NGX_C_CONF_H
