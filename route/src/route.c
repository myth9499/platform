#include"struct.h"
#include"route.h"

_ROUTECFG *getrtcfg(char *);
void *myprocess(void *arg);
int main(int argc,char *argv[])
{
    int sockfd,clifd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    int sin_size = 0;
    char rcvbuf[4096];
    char tranid[5];
    _ROUTECFG *rtcfg = NULL;
    rtcfg = (_ROUTECFG *)malloc(sizeof(_ROUTECFG));

    memset(rcvbuf,0,sizeof(rcvbuf));
    memset(tranid,0,sizeof(tranid));

	/** 设置忽略SIGPIPE信号，防止因socket写的时候客户端关闭导致的SIGPIPE信号 **/
	//signal(SIGPIPE,SIG_IGN);
	#ifndef WIN32
	sigset_t signal_mask;
	sigemptyset (&signal_mask);
	sigaddset (&signal_mask, SIGPIPE);
	int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
	if (rc != 0) {
		printf("block sigpipe error/n");
	} 
	#endif  
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("创建服务器套接字失败");
        return -1;
    }
    printf("创建服务器套接字成功\n");
    bzero(&serv_addr,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10000);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr *)(&serv_addr),sizeof(struct sockaddr))==-1)
    {
        perror("bind 失败");
        return -1;
    }
    if(listen(sockfd,100)==-1)
    {
        perror("listen 失败");
        return -1;
    }
    /**初始化线程池 **/
    pool_init(100);
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        /**clifd =(int *)malloc(sizeof(int));
        if(clifd == NULL)
        {
            perror("malloc clifd error");
            return -1;
        }
		**/
        if((clifd = accept(sockfd,(struct sockaddr *)(&cli_addr),&sin_size))==-1)
        {
            perror("accept 失败");
            return -1;
        }
        printf("获取来自[%s]的连接 sockfd is[%d]\n",inet_ntoa(cli_addr.sin_addr),clifd);
        if(pool_add_worker(myprocess,&clifd)!=-1)
		{
			printf("添加工作任务成功\n");
		}else
		{
			printf("添加工作任务失败,请检查是否已达到初始线程最大值\n");
			send(clifd,"添加工作任务失败,请检查是否已达到初始线程最大值",strlen("添加工作任务失败,请检查是否已达到初始线程最大值"),0);
			close(clifd);
			sleep(1);
		}
        /**
        if(recv(clifd,rcvbuf,sizeof(rcvbuf),0)==-1)
        {
        	perror("read error");
        	return -1;
        }
        printf("rcvbuf is [%s]\n",rcvbuf);
        memcpy(tranid,rcvbuf,4);
        if((rtcfg = getrtcfg(tranid))==NULL)
        {
        	printf("共享内存中未获取到该交易[%s]的配置信息\n",tranid);
        	continue;
        	//return -1;
        }
         根据共享内存区的配置信息，连接其他系统

        int rtsockfd;
        struct sockaddr_in rtserv_addr;
        if((rtsockfd =socket(AF_INET,SOCK_STREAM,0))==-1 )
        {
        	perror("get remote socketfd error");
        	return -1;
        }
        printf("[---%s]----\n",rtcfg->tranmask);

        bzero(&rtserv_addr,sizeof(rtserv_addr));
        rtserv_addr.sin_family = AF_INET;
        rtserv_addr.sin_port = htons(atoi(rtcfg->servport));
        rtserv_addr.sin_addr.s_addr=inet_addr(rtcfg->servip);
        if(connect(rtsockfd,(struct sockaddr *)(&rtserv_addr),sizeof(struct sockaddr))==-1)
        {
        	printf("connect remote server[%s] error[%s]\n",rtcfg->servip,strerror(errno));
        	return -1;
        }
        printf("connect to server[%s] ok \n",rtcfg->servip);
        if(write(clifd,rtcfg->tranmask,strlen(rtcfg->tranmask))==-1)
        {
        	perror("写返回数据失败");
        	return -1;
        }
        sleep (2);
        close(clifd);
        **/
    }
	fprintf(stderr,"%s\n","|||||||||||||||||exit");
    close(sockfd);
    pool_destory();
    return 0;
}
