/*************************************************************************
	> File Name: server.h
	> Author: NoMi
	> Mail: 1559674040@qq.com
	> Created Time: 2019年08月19日 星期一 19时48分29秒
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<errno.h>
#include<sqlite3.h>

#define NAMESIZE 64
#define DATASIZE 128
#define BUFSIZE 200
#define SERV_ADDR "192.168.133.129"
#define SERV_PORT 7294
#define BACKLOG 5

typedef struct{
    char type;
    char name[NAMESIZE];
    char data[DATASIZE];
}MSG;

struct sockaddr_in clt;
char buf[BUFSIZE];
int tcp_init();
int deal_client();
void sender();
void recver();
void do_register();
void do_login();
void do_query();
void do_edit();
int init_database();
#endif
