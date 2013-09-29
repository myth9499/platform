#include "platform.h"
#include "struct.h"

int send_to_serv(char *servtype,char *buffer)
{
    int shmid = 0,i=0;
    key_t key;

	_transhm	*trandata=NULL;
	_servershm	*server=NULL;
	int maxtran = 10;
	int maxserver = 50;

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

	/**查找未被使用的交易内存区 **/
	for(i=0;i<maxtran;i++)
	{
		if((trandata+i)->s_pid==0)
		{
			/**test **/
			memset(trandata+i,0X00,sizeof(_transhm));

			/** 此处需要共享内存同步 **/
			(trandata+i)->s_pid = (pid_t)getpid();
			strcpy((trandata+i)->send_chnl,"QDTX");
			strcpy((trandata+i)->chnl_data,buffer);
			strcpy((trandata+i)->cur_stat,"W");
			strcpy((trandata+i)->require_serv,"Q");

			/** 根据交易信息,获取服务PID,发送信号 **/
			for(int j=0;j<50;j++)
			{
				printf("%ld %s %s\n",(server+j)->serverpid,(server+j)->require_serv,(server+j)->servtype);
				if((server+j)->serverpid!=0&&!strcmp((server+j)->require_serv,servtype)&&!strcmp((server+j)->servtype,"S"))
				{
					kill((server+j)->serverpid,SIGUSR2);
					return i;
					break;
				}
				if(j==49)
				{
					printf("该交易没有对应核心服务\n");
					return -1;
				}
			}
		}
	}
	printf("共享内存全部在使用中...\n");
	return -1;
}
