/*************************************************************************
  > File Name: server.c
  > Author: NoMi
  > Mail: 1559674040@qq.com
  > Created Time: 2019年08月19日 星期一 16时09分59秒
 ************************************************************************/

#include"server.h"

extern int cfd;
extern MSG msg;
int nrow = 0;
int i = 0;
int ncolumn = 0;
char *zErrmsg = NULL;
char **azErrmsg = NULL;
sqlite3 *db = NULL;
int tcp_init()
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > sfd){
        return -1;
    }
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in ser;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(SERV_PORT);
    ser.sin_addr.s_addr = inet_addr(SERV_ADDR);

    if(bind(sfd, (struct sockaddr*)&ser, sizeof(ser)) < 0){
        return -2;
    }

    if(listen(sfd, BACKLOG) < 0){
        return -3;
    }

    return sfd;
}

int deal_client()
{
    memset(&msg, 0, sizeof(msg));
    recver();
    switch(msg.type)
    {
        case 'R'://register
            do_register();
            break;
        case 'L'://login
            do_login();
            break;
        case 'Q'://query word
            do_query();
            break;
        case 'M'://modify infomation
            do_edit();
            break;
        case 'O'://login out
            return -1;
            break;
        default:
            msg.type = 'E';
            sender();
            break;
    }
    return 0;
}

void sender()
{
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%c#%s#%s", msg.type, msg.name, msg.data);
    if(write(cfd, buf, BUFSIZE-1) < 0){
        perror("无法发送信息到客户端...\n原因");
        exit(1);
    }
    return;
}

void recver()
{
    memset(&msg, 0, sizeof(msg));
    if(read(cfd, buf, BUFSIZE-1) < 0){
        perror("无法读取客户端数据...\n原因");
        exit(1);
    }
    msg.type = buf[0];
    char *Name = &buf[2];
    char *Data = strstr(&buf[2], "#");
    *Data = '\0';
    strcpy(msg.name, Name);
    strcpy(msg.data, Data+1);
    return;
}

void do_register()
{
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select pawd from USERS_TABLE where name == '%s'", msg.name);
    sqlite3_get_table(db, buf, &azErrmsg, &nrow, &ncolumn, &zErrmsg);
    if(nrow >= 1){
        msg.type = 'E';
        strcpy(msg.name, "liuxun");
        strcpy(msg.data, "用户已经存在！");
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "INSERT INTO 'USERS_TABLE' VALUES('%s', '%s');", msg.name, msg.data);
        sqlite3_exec(db, buf, NULL, NULL, &zErrmsg);
        msg.type = 'S';
    }
    sender();
    return;
}

void do_login()
{
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select pawd from USERS_TABLE where name == '%s'", msg.name);
    sqlite3_get_table(db, buf, &azErrmsg, &nrow, &ncolumn, &zErrmsg);
    if(1 > nrow){
        printf("no user\n");
        msg.type = 'E';
        strcpy(msg.data, "该用户不存在，请确认输入！");
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select pawd from USERS_TABLE where name == '%s'", msg.name);
        sqlite3_get_table(db, buf, &azErrmsg, &nrow, &ncolumn, &zErrmsg);
        if(!strncasecmp(azErrmsg[1], msg.data, strlen(msg.data))){
            msg.type = 'S';
            strcpy(msg.data, "登录成功！");
        }
        else{
            msg.type = 'E';
            strcpy(msg.data, "密码错误，请确认输入！");
        }
    }
    sender();
    return;

    sender();
    return;
}

void do_query()
{
    int find = 0;
    FILE *p = fopen("./dict.txt", "r+");
    if(NULL == p){
        perror("fopen");
        exit(-1);
    }
    fseek(p, 0, SEEK_SET);
    while(NULL != fgets(msg.data, sizeof(msg.data), p))
    {
        if(!strncasecmp(msg.name, msg.data, strlen(msg.name))){
            find = 1;
            break;
        }
    }
    if(!find){
        sprintf(msg.data, "%s         未查询到该单词！\n", msg.name);
    }
    sender();
    return;
}

void do_edit()
{
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "INSERT INTO 'USERS_TABLE' VALUES('%s', '%s');", msg.name, msg.data);
    sqlite3_exec(db, buf, NULL, NULL, &zErrmsg);
    msg.type = 'S';
    sender();
    return;
}

int init_database()
{
    int ret = sqlite3_open("user", &db);
    if(ret){
        fprintf(stderr, "Cant' open database:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    printf("database open successfully!\n");

    char *sql = "CREATE TABLE USERS_TABLE(\
                 name VARCHAR(20),\
                 pawd VARCHAR(19));";
    sqlite3_exec(db, sql, NULL, NULL, &zErrmsg);
    printf("%s\n",zErrmsg);
    sqlite3_free(zErrmsg);
    char *sql1 = "INSERT INTO 'USERS_TABLE' VALUES('heting', '520');";
    sqlite3_exec(db, sql1, NULL, NULL, &zErrmsg);
    char *sql2 = "INSERT INTO 'USERS_TABLE' VALUES('liuxun', '123456');";
    sqlite3_exec(db, sql2, NULL, NULL, &zErrmsg);

    return 0;
}
