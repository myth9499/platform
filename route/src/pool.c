#include"struct.h"
#include"route.h"
#include"pool.h"

/*
 *Linux 下C 线程池函数
 *李磊 2013年1月21日
 *
*/
void *thread_routine(void *arg);
int pool_add_worker(void *(*process)(void *arg),void *arg);
void *thead_routine(void *arg);
static CThread_pool *pool=NULL;
_ROUTECFG *getrtcfg(char *);


void pool_init(int max_thread_num)
{
    pool = (CThread_pool *)malloc(sizeof(CThread_pool));

    pthread_mutex_init(&(pool->queue_lock),NULL);
    pthread_cond_init(&(pool->queue_ready),NULL);

    pool->queue_head=NULL;

    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;

    pool->shutdown = 0;

    pool->threadid = (pthread_t *)malloc(max_thread_num*sizeof(pthread_t));
    int i = 0;
    for(i=0; i<max_thread_num; i++)
    {
        pthread_create(&(pool->threadid[i]),NULL,thread_routine,NULL);
    }
}

int pool_add_worker(void *(*process)(void *arg),void *arg)
{
	if(pool->cur_workcnt>=pool->max_thread_num)
	{
		printf("目前队列深度[%d]\n",pool->cur_workcnt);
    	//pthread_mutex_unlock(&(pool->queue_lock));
    	//pthread_cond_signal(&(pool->queue_ready));
		return -1;
	}
    CThread_worker *newworker = (CThread_worker *)malloc(sizeof(CThread_worker));
    newworker->process = process;
    newworker->arg = arg;
    newworker->next = NULL;

    pthread_mutex_lock(&(pool->queue_lock));
    CThread_worker *member = pool->queue_head;
    if(member!=NULL)
    {
        while(member->next!=NULL)
        {
            member = member->next;
        }
        member->next = newworker;
    } else
    {
        pool->queue_head = newworker;
    }
    assert(pool->queue_head!=NULL);
    pool->cur_queue_size++;
    pool->cur_workcnt++;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));
    return 0;
}

int pool_destory()
{
    if(pool->shutdown)
        return -1;
    pool->shutdown = 1;

    pthread_cond_broadcast(&(pool->queue_ready));

    int i ;
    for(i=0; i<pool->max_thread_num; i++)
    {
        pthread_join(pool->threadid[i],NULL);
    }
    free(pool->threadid);

    CThread_worker *head = NULL;
    while(pool->queue_head!=NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(head);
    }

    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    free(pool);
    pool = NULL;
    return 0;
}


void *thread_routine(void *arg)
{
    printf("starting thread 0x%x\n",pthread_self());
    while(1)
    {
        pthread_mutex_lock(&(pool->queue_lock));
        while(pool->cur_queue_size == 0 && !pool->shutdown)
        {
            printf("thread 0x%x is waitting \n",pthread_self());
            pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
        }
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->queue_lock));
            printf("thread 0x%x will exit\n",pthread_self());
            pthread_exit(NULL);
        }
        printf("thread 0x%x is starting to work \n",pthread_self());
        assert(pool->cur_queue_size!=0);
        assert(pool->queue_head!=NULL);

        pool->cur_queue_size -- ;
        CThread_worker *worker = pool->queue_head;
        pool->queue_head = worker->next;
        pthread_mutex_unlock(&(pool->queue_lock));
        (*(worker->process))(worker->arg);
        free(worker);
        worker = NULL;
        pthread_mutex_lock(&(pool->queue_lock));
        pool->cur_workcnt -- ;
        pthread_mutex_unlock(&(pool->queue_lock));
    }
    pthread_exit(NULL);
}

void *myprocess(void *arg)
{
    char rcvbuf[4096];
    int clifd = 0;
    clifd = *(int *)arg;
    //free(arg);
    char tranid[5];
    char tranvalue[4096];
    _ROUTECFG *rtcfg=NULL;
    char errmsg[1024];
    char rtmsg [1024];

	/** 为了监控 **/
	key_t key2;
	key2 = ftok("/item/platform/route/etc/SHMKEY",12);
	if(key2 == (key_t)-1)
	{
		perror("ftok key error");
		return NULL;
	}
	printf("ftok ok\n");
	int shmid  = 0;
	_SHMMON *shmmon = NULL;
    shmid = shmget(key2,MAXTRAN*sizeof(_SHMMON),IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
		return NULL;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
    shmmon = (_SHMMON *)shmat(shmid,NULL,0);
    if(shmmon == (void *)-1)
    {
        perror("shmat error");
        return NULL;
    }
	/** 为了监控**/

    memset(tranid,0,sizeof(tranid));
    memset(tranvalue,0,sizeof(tranvalue));
    memset(rcvbuf,0,sizeof(rcvbuf));
    memset(errmsg,0,sizeof(errmsg));
    memset(rtmsg,0,sizeof(rtmsg));
    printf("thread is 0x%x,working on task sockfd[%d]\n",pthread_self(),clifd);
    if(recv(clifd,rcvbuf,sizeof(rcvbuf),0)==-1)
    {
        perror("read error");
        strcpy(errmsg,"read clifd error");
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
        return NULL;
    }
    printf("rcvbuf is [%s]\n",rcvbuf);
    memcpy(tranid,rcvbuf,4);
	if(rcvbuf[4]!='|')
	{
        strcpy(errmsg,"交易内容格式错");
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
		return NULL;
	}
	/** 为了监控，修改处理数字  此处需要处理共享内存同步**/
	while(strcmp(shmmon->tranid,"END"))
	{
		if(!strcmp(shmmon->tranid,tranid))
		{
			shmmon->rightcnt++;
			break;
		}
		shmmon++;
	}
	/** 为了监控，修改处理数字  **/
    if((rtcfg = getrtcfg(tranid))==NULL)
    {
        printf("共享内存中未获取到该交易[%s]的配置信息\n",tranid);
        strcpy(errmsg,"共享内存中未获取到该交易的配置信息");
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        printf("after perror\n");
        //continue;
        //sleep (1);
        close(clifd);
        return NULL;
    }
    /** 根据共享内存区的配置信息，连接其他系统 **/

    int rtsockfd;
    struct sockaddr_in rtserv_addr;
    if((rtsockfd =socket(AF_INET,SOCK_STREAM,0))==-1 )
    {
        perror("get remote socketfd error");
        sprintf(errmsg,"%s:%s","get remote sockfd error",strerror(errno));
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
        close(rtsockfd);
        return NULL;
    }
    printf("[---%s]----\n",rtcfg->tranmask);

    bzero(&rtserv_addr,sizeof(rtserv_addr));
    rtserv_addr.sin_family = AF_INET;
    rtserv_addr.sin_port = htons(atoi(rtcfg->servport));
    rtserv_addr.sin_addr.s_addr=inet_addr(rtcfg->servip);
    if(connect(rtsockfd,(struct sockaddr *)(&rtserv_addr),sizeof(struct sockaddr))==-1)
    {
        printf("connect remote server[%s] error[%s]\n",rtcfg->servip,strerror(errno));
        sprintf(errmsg,"%s:%s","connect to the remote server error",strerror(errno));
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
        close(rtsockfd);
        return NULL;
    }
	strcpy(tranvalue,rcvbuf+5);
    printf("connect to server[%s] ok tranvalue is [%s]\n",rtcfg->servip,tranvalue);
	if(send(rtsockfd,tranvalue,sizeof(tranvalue),0)==-1)
	{
        sprintf(errmsg,"%s:%s","发送到远端服务器失败",strerror(errno));
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
        close(rtsockfd);
	}
	printf("发送到远端服务器成功,等待接收...\n");
    if(recv(rtsockfd,rtmsg,sizeof(rtmsg),0)==-1)
    {
        sprintf(errmsg,"%s:%s","receive remote msg error",strerror(errno));
        if(send(clifd,errmsg,strlen(errmsg),0)==-1)
            perror("write error");
        //sleep (1);
        close(clifd);
        close(rtsockfd);
        return NULL;
    }
    printf("rtmsg is [%s]\n",rtmsg);
	//sleep(5);
    if(send(clifd,rtmsg,strlen(rtmsg),0)!=strlen(rtmsg))
    {
        perror("写返回数据失败");
        //sleep (1);
        close(clifd);
        close(rtsockfd);
        return NULL;
    }
    //sleep (1);
    close(clifd);
    close(rtsockfd);
    printf("proc ok\n");
    return NULL;
}
