//
// Created by xqq on 24-6-6.
//

#include "MqttCommon.h"

bool util::check_topic_match(const std::string &pubTopic, const std::string &subTopic) {
    uint32_t pubLen = pubTopic.length();
    uint32_t subLen = subTopic.length();
    uint32_t i = 0, j = 0;
    if (pubLen == 0 || subLen == 0) {
        return true;
    }
    while (i < pubLen && j < subLen) {
        if (pubTopic[i] == subTopic[j]) {
            i++;
            j++;
            continue;
        }
        if (subTopic[j] == '+') {
            if (pubTopic[i] == '/') {
                if (j + 1 < subLen) {
                    i++;
                    j += 2;    // 后一位一定是 '/', 需要跨过
                } else {
                    return false;
                }
            } else {
                // 匹配一个层级
                while (i < pubLen && pubTopic[i] != '/') {
                    i++;
                }

                if (i == pubLen) {
                    if (j == subLen - 1) {
                        // 最后一个层级匹配
                        return true;
                    }

                    // example:
                    // pub("xx") sub("+/#")
                    if (j + 2 < subLen && subTopic[j + 2] == '#') {
                        return true;
                    }

                    return false;
                }

                // 本层匹配完成, 进入下一层
                if (j + 1 < subLen) {
                    i++;
                    j += 2;
                }
            }
        } else if (subTopic[j] == '#') {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
