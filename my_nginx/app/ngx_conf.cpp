//
// Created by Administrator on 2024-03-07.
//
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ngx_func.h"
#include "app/ngx_c_conf.h"
#include "app/ngx_string.h"

using namespace std;

CConfig * CConfig::m_instance = nullptr;

CConfig::CConfig() {

}

CConfig::~CConfig() {
    vector<LPCConfItem>::iterator  pos;
    for(pos = mConfigItemList.begin(); pos != mConfigItemList.end(); ++pos){
        delete (*pos);
    }
    mConfigItemList.clear();
}

bool CConfig::load(std::string_view pConfName) {
    if(pConfName.empty()){
        return false;
    }
    FILE *fp;
    fp = fopen(pConfName.data(), "r");
    if (fp == nullptr)
        return false;
    char lineBuf[501] ={0};

    while (!feof(fp)) {

        if (fgets(lineBuf, 500, fp) == nullptr)
            continue;

        if (lineBuf[0] == 0)
            continue;

        if (*lineBuf == ';' || *lineBuf == ' ' || *lineBuf == '#' || *lineBuf == '\t' || *lineBuf == '\n')
            continue;


        //remove character \r \n  space from the end of line
        for(;;){
            if(lineBuf[strlen(lineBuf)-1] == '\n' || lineBuf[strlen(lineBuf) -1] == '\r' || lineBuf[strlen(lineBuf-1)] == ' '){
                lineBuf[strlen(lineBuf) -1] = 0;
            }else{
                break;
            }
        }

        if(lineBuf[0] == 0)
            continue;
        if(*lineBuf == '[')
            continue;

        char* ptmp =  strchr(lineBuf,'=');

        if(ptmp != nullptr){
            LPCConfItem lpConfItem = new CConfItem ;
            memset(lpConfItem, 0, sizeof(LPCConfItem));
            strncpy(lpConfItem->ItemName, lineBuf, (int)(ptmp - lineBuf));
            strcpy(lpConfItem->ItemContent, ptmp + 1);

            Rtrim(lpConfItem->ItemName);
            Ltrim(lpConfItem->ItemName);
            Rtrim(lpConfItem->ItemContent);
            Ltrim(lpConfItem->ItemContent);
            mConfigItemList.push_back(lpConfItem);
        }

    }
    fclose(fp);
    return true;
}

const std::optional<std::string> CConfig::getString(std::string_view pItemName) {
    for(const auto& item : mConfigItemList){
        if(strcasecmp(item->ItemName,pItemName.data()) == 0){
            return item->ItemContent;
        }
    }
    return nullopt;
}

optional<int> CConfig::getIntDefault(std::string_view pItemName, const int def) {
    try{
        for(const auto& item : mConfigItemList){
            if(strcasecmp(item->ItemName,pItemName.data()) == 0){
                string  value = item->ItemContent;
                return stoi(value);
            }
        }
    }catch (...){
    }
    return def;
}
