//
// Created by Administrator on 2024-03-08.
//

#ifndef MYNGINX_NGX_GLOBAL_H
#define MYNGINX_NGX_GLOBAL_H

typedef struct
{
    char ItemName[50];
    char ItemContent[500];
}CConfItem, *LPCConfItem;

extern char **g_os_argv;
extern char * gp_envmem;
extern int g_envirolen;

extern pid_t ngx_pid;
extern pid_t ngx_parent;

#endif //MYNGINX_NGX_GLOBAL_H
