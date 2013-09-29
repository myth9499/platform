#include "platform.h"
#include "struct.h"

_transhm    *trandata=NULL;
_servershm  *server=NULL;
int maxtran = 10;
int maxserver = 50;

void serv(int sig)
{
	printf("捕获信号\n");
	int i = 1;
	while(i<maxtran)
	{
		if(!strcmp((trandata+i-1)->cur_stat,"W"))
		{
			strcpy((trandata+i-1)->cur_stat,"A");
			(trandata+i-1)->a_pid = (pid_t)88888888;
			printf("核心[%ld]开始处理来自渠道[%s][%ld]的交易[%s]\n",(trandata+i-1)->a_pid,(trandata+i-1)->send_chnl,(trandata+i-1)->s_pid,(trandata+i-1)->chnl_data );
			strcpy((trandata+i-1)->return_chnl,"TEST OK");
			strcpy((trandata+i-1)->cur_stat,"E");
			memset(trandata+i-1,0X00,sizeof(_transhm));
			break;
		}
		i++;
	}
	signal(SIGUSR2,serv);
}
int main(int argc,char *argv[])
{
    int shmid = 0;
    key_t key;


	/** 需要修改为从配置文件读取 **/
    key = ftok("/item/platform/route/etc/SHMKEY",11);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,maxtran*sizeof(_transhm),IPC_EXCL);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
	if((trandata=shmat(shmid,NULL,0))==(void *)-1)
	{
		perror("shmat error");
		return -1;
	}

	key = ftok("/item/platform/route/etc/SHMKEY",14);
	if(key == (key_t)-1)
	{
		printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
		return -1;
	}
	printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
	shmid = shmget(key,maxserver*sizeof(_servershm),IPC_EXCL);
	if(shmid == -1)
	{
		perror("shm get error");
		return -1;
	}
	printf("shmget ok shmid is [%d]\n",shmid);
	if((server=shmat(shmid,NULL,0))==(void *)-1)
	{
		perror("shmat error");
		return -1;
	}
	for(int n=0;n<50;n++)
	{
		if((server+n)->serverpid==0)
		{
			/**此处需要共享内存同步**/
			(server+n)->serverpid = getpid();
			strcpy((server+n)->servtype,"S");
			strcpy((server+n)->require_serv,"HK");
			strcpy((server+n)->chnlid,"SRV1");
			(server+n)->cnt = 10;
			break;
		}
		if(n==49)
		{
			printf("没有足够server共享内存\n");
			return -1;
		}
	}

	/** 注册信号监控 **/

	signal(SIGUSR2,serv);
	while(1)
	{
		pause();
	}
	return 0;
}
