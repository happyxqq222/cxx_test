//
// Created by Administrator on 2024-03-25.
//

#ifndef MY_MUDUO_TOOL_H
#define MY_MUDUO_TOOL_H

#define ERROR_CHECK(arg, errcode, errdescri){ \
          if(arg == errcode){                    \
             printf("%s\n",errordescri);        \
             exit(-1);                                   \
          }                                   \
        }
#endif //MY_MUDUO_TOOL_H
