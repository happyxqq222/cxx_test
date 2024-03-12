//
// Created by Administrator on 2024-03-12.
//

#include <cstdint>
#include <cstring>
#include "log/Log.h"
#include "stdarg.h"

void ngx_log_stderr(int err,const char *fmt,...)
{
    va_list  args;
    int8_t errstr[NGX_MAX_ERROR_STR+1];
    int8_t *p, *last;
    memset(errstr,0,sizeof(errstr));
    last = errstr + NGX_MAX_ERROR_STR;

}

static u_char* ngx_sprintf_num(u_char* buf, u_char* last,uint64_t ui64,u_char zero, uintptr_t hexadcimal,uintptr_t width)
{

}
