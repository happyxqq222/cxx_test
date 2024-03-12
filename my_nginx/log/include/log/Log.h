//
// Created by Administrator on 2024-03-12.
//

#ifndef MYNGINX_LOG_H
#define MYNGINX_LOG_H

#define NGX_MAX_ERROR_STR 2048

#define ngx_cpymem(dst,src,n)   (((u_char*)memcpy(dst,src,n))+(n))

typedef unsigned char u_char;
typedef unsigned int u_int;

#define NGX_MAX_UINT32_VALUE (uint32_t)0xffffffff
#define NGX_INT64_LEN   (sizeof("-9223372036854775808") - 1)

void ngx_log_stderr(int err,const char *fmt,...);

static u_char* ngx_sprintf_num(u_char* buf, u_char* last,uint64_t ui64,u_char zero, uintptr_t hexadcimal,uintptr_t width);


#endif //MYNGINX_LOG_H
