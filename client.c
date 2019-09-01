/*************************************************************************
	> File Name: client.c
	> Author: NoMi
	> Mail: 1559674040@qq.com
	> Created Time: 2019年08月19日 星期一 15时34分44秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/in.h>

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

int sfd = -1;
MSG msg;
char buf[BUFSIZE];
char sys_username[NAMESIZE];

void tcp_init();
void clt_run();
void clt_register();
void clt_login();
void clt_exit();
void sender();
void recver();
void sys_run();
void clt_query();
void clt_history();
void clt_Minfo();

/*############## main #######################*/
int main(int argc, char *argv[])
{
    tcp_init();
    clt_run();
    return 0;
}
/*############### function ###################*/

void tcp_init()
{
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > sfd){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in ser;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(SERV_PORT);
    ser.sin_addr.s_addr = inet_addr(SERV_ADDR);

    if(connect(sfd, (struct sockaddr*)&ser, sizeof(ser)) < 0){
        perror("connect");
        exit(-1);
    }
    
}

void clt_run()
{
    printf("\x1b[H\x1b[2J");
    printf("##########################################################\n");
    printf("#                 在线翻译系统                           #\n");
    printf("##########################################################\n");
    printf("#           1:注册   2：登录    3：退出                  #\n");
    printf("##########################################################\n");
    printf("请选择>");

    char flag = -1;
    scanf("%c", &flag);
    switch(flag)
    {
        case '1':
            clt_register();
            break;
        case '2':
            clt_login();
            break;
        case '3':
            clt_exit();
            break;
        default:
            printf("输入有误，请重新选择...\n");
            sleep(1);
            clt_run();
    }

}

void clt_register()
{
    memset(&msg, 0, sizeof(msg));
    msg.type = 'R';
    int ret = -1;
    printf("\x1b[H\x1b[2J");
    printf("******************************\n");
    printf("用户注册...                   \n");
    printf("******************************\n");
    printf("name:>");
    scanf("%s", msg.name);
    printf("pawd:>");
    scanf("%s", msg.data);
    sender();
    recver();
    if(msg.type == 'S'){
        printf("注册成功，请返回登录！\n");
        sleep(3);
        clt_run();
    }
    else if(msg.type == 'E'){
        printf("注册失败：%s\n", msg.data);
        sleep(3);
        clt_run();
    }
    return;
}

void clt_login()
{
    memset(&msg, 0, sizeof(msg));
    msg.type = 'L';
    printf("\x1b[H\x1b[2J");
    printf("******************************\n");
    printf("用户登录...                   \n");
    printf("******************************\n");
    printf("name>");
    scanf("%s", msg.name);
    printf("pawd>");
    scanf("%s", msg.data);

    sender();
    recver();
    if(msg.type == 'S'){
        printf("登录成功！\n");
        strncpy(sys_username, msg.name, NAMESIZE);
        sleep(1);
        sys_run();
    }
    else{
        printf("登录失败：%s\n", msg.data);
        sleep(1);
        clt_run();
    }
    return;

}

void clt_exit()
{
    memset(&msg, 0, sizeof(msg));
    msg.type = 'O';
    strcpy(msg.name, sys_username);
    sender();
    printf("\x1b[H\x1b[2J");
    printf("\n\n\n\n\n\n");
    printf("############################################################\n");
    printf("#                     谢谢使用！                           #\n");
    printf("#                 powered by liuxun                        #\n");
    printf("############################################################\n");
    printf("\n\n\n\n\n\n\n");
    return;
}

void sender()
{
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%c#%s#%s", msg.type, msg.name, msg.data);
    if(write(sfd, buf, BUFSIZE-1) < 0){
        perror("无法发送请求到服务器...\n原因");
    exit(1);
    }
    return;
}

void recver()
{
    memset(&msg, 0, sizeof(msg));
    memset(buf, 0, sizeof(buf));
    int ret = read(sfd, buf, BUFSIZE-1);
    if(ret < 0){
        perror("无法向服务器获取返回数据...\n原因");
        exit(1);
    }
    else if(0 == ret){
        recver();
    }
    msg.type = buf[0];
    char *Name = &buf[2];
    char *Data = strstr(&buf[2], "#");
    *Data = '\0';
    strcpy(msg.name, Name);
    strcpy(msg.data, Data+1);
    return;
}

void sys_run()
{
    sleep(1);
    int flag = -1;
    printf("\x1b[H\x1b[2J");
    printf("############################################################\n");
    printf("#                   在线翻译系统                           #\n");
    printf("############################################################\n");
    printf("#  1:查询单词   2：历史记录    3：帐号管理   4：注销登录   #\n");
    printf("############################################################\n");
    printf("请选择>");
    scanf("%d", &flag);
    switch(flag)
    {
        case 1:
            clt_query();
            break;
        case 2:
            clt_history();
            break;
        case 3:
            clt_Minfo();
            break;
        case 4:
            clt_run();
            break;
        default:
            printf("输入有误，请重新选择...\n");
            sys_run();
            break;

    }
    return;
}

void clt_query()
{
    char buf[NAMESIZE-1];
    printf("\x1b[H\x1b[2J");
    printf("*******************************************\n");
    printf("查询单词...                       >:退出  \n");
    printf("*******************************************\n");
    while(1)
    {
        memset(&msg, 0, sizeof(msg));
        msg.type = 'Q';
        memset(buf,0, sizeof(buf));
        printf(">");
        fflush(stdout);
        scanf("%s", buf);
        if(!strncasecmp(buf, ">", strlen(">"))){
            sys_run();
            break;
        }
        strcpy(msg.name, buf);
        sender();
        recver();
        printf("%s\n", msg.data);
        FILE *p = fopen("./history.txt", "a+");
        if(NULL == p){
            perror("fopen");
            exit(1);
        }
        int ret = fputs(msg.data, p);
        if(0 > ret){
            perror("fputs");
            exit(1);
        }
        fclose(p);
    }
    
    return;
}

void clt_history()
{
    printf("\x1b[H\x1b[2J");
    printf("************************************************\n");
    printf("历史记录...                             >:退出  \n");
    printf("************************************************\n");
    FILE *p = fopen("./history.txt", "r");
    if(NULL == p){
        printf("未查询到历史记录！\n");
        sleep(2);
        sys_run();
    }
    fseek(p, 0, SEEK_SET);
    printf("单词            词义\n");
    memset(buf, 0, sizeof(buf));
    while(NULL != fgets(buf, sizeof(buf), p))
    {
        printf("%s\n", buf);
    }
    fclose(p);
    char ch;
    printf("查询结束,>退出...\n>");
    getchar();
    scanf("%c", &ch);
    if('>' == ch){
        sys_run();
    }
    return;
}

void clt_Minfo()
{
    memset(&msg, 0, sizeof(buf));
    char buf[NAMESIZE-1];
    msg.type = 'M';
    printf("\x1b[H\x1b[2J");
    printf("************************************************\n");
    printf("帐号管理...                          >:退出     \n");
    printf("************************************************\n");
    printf("编辑信息:\n");
    printf("name:");
    fflush(stdout);
    scanf("%s", msg.name);
    if(!strncasecmp(msg.name, ">", strlen(">"))){
        sys_run();
        return;
    }
    printf("pawd:");
    fflush(stdout);
    scanf("%s", msg.data);
    sender();
    recver();
    if(msg.type == 'S'){
        printf("账户修改成功！\n");
        sleep(2);
        sys_run();
    }
    else if(msg.type == 'E'){
        printf("账户修改失败！\n");
        sleep(2);
        sys_run();
    }
    return;
}
