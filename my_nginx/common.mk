ifeq ($(DEBUG), true)
CC = gcc -g
VERSION = debug
else
CC = gcc
VERSION = release
endif


# $(wildcard *.c)
#SRCS = nginx.c ngx_conf.c
SRCS = $(wildcard *.cpp)

#OBJS = nginx.o ngx_conf.o  
OBJS = $(SRCS:.cpp=.o)

#���ַ����е�.cpp�滻Ϊ.d
DEPS = $(SRCS:.c=.d)

#����ָ��BIN�ļ���λ��,addprefix������ǰ׺����
#BIN = /mnt/hgfs/linux/nginx
BIN := $(addprefix $(BUILD_ROOT)/.$(BIN))

