//
// Created by Administrator on 2024-03-07.
//
#include <stdio.h>
#include <unistd.h>

#include "ngx_func.h"  //ͷ�ļ�·�����Ѿ�ʹ��gcc -I����ָ����
#include "ngx_signal.h"

int main(int argc, char *const *argv)
{
    printf("�ǳ����ˣ���Һ���ʦһ��ѧϰ��linux c++ͨѶ�ܹ�ʵս��\n");
    myconf();
    mysignal();

    /*for(;;)
    {
        sleep(1); //��Ϣ1��
        printf("��Ϣ1��\n");
    }*/
    printf("�����˳����ټ�!\n");
    return 0;
}