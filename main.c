/*************************************************************************
	> File Name: main.c
	> Author: NoMi
	> Mail: 1559674040@qq.com
	> Created Time: 2019年08月19日 星期一 19时48分39秒
 ************************************************************************/


#include"server.h"

int cfd = -1;
MSG msg;

int main(int argc, char *argv[])
{
    int sfd = tcp_init();
    if(0 > sfd){
        perror("tcp_init");
        return -1;
    }
    init_database();
    printf("服务器已开启...\n");
    memset(&clt, 0, sizeof(clt));
    socklen_t clt_len = sizeof(clt);
    pid_t pid = -1;

    while(1)
    {
        sleep(3);
        cfd = accept(sfd, (struct sockaddr*)&clt, &clt_len);
        if(0 > cfd){
            perror("accept");
            continue;
        }
        if((pid = fork()) < 0){
            perror("fork");
            return -1;
        }
        else if(pid == 0)
        {
            close(sfd);
            printf("用户%s已接入....\n", inet_ntoa(clt.sin_addr));
            fflush(stdout);
            while(1)
            {
                if(deal_client() < 0){
                    printf("用户%s已退出....\n", inet_ntoa(clt.sin_addr));
                    break;
                }
            }
            exit(0);
        }
        close(cfd);
    }

    return 0;
}
