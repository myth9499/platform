#include "platform.h"
#include "struct.h"

int send_to_serv(char *,char *);
int main(int argc,char *argv[])
{
    int shmid = 0,ret=0;
    key_t key;

	_transhm	*trandata=NULL;
	_servershm	*server=NULL;
	int maxtran = 10;
	int maxserver = 50;
	char type[2];

	memset(type,0X00,sizeof(type));

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

	/**此处需要共享内存同步**/
	for(int n=0;n<50;n++)
	{
		if((server+n)->serverpid==0)
		{
			(server+n)->serverpid = getpid();
			strcpy((server+n)->servtype,"C");
			strcpy((server+n)->require_serv,"HK");
			strcpy((server+n)->chnlid,"QDTX");
			(server+n)->cnt =10;
			break;
		}
		if(n==49)
		{
			printf("没有足够server共享内存\n");
			return -1;
		}
	}

	/**根据参数,判断该渠道是发起渠道还是接收渠道**/

	strcpy(type,argv[1]);
	if(!strcmp(type,"S"))
	{

		if((ret = send_to_serv("HK","test send to sever"))!=-1)
		{
			printf("发送到核心成功,等待核心返回......\n");
			while(1)
			{
				if(strcmp((trandata+ret)->return_chnl,""))
				{
					printf("核心处理成功,返回数据[%s]\n",(trandata+ret)->return_chnl);
					return 0;
				}
			}
		}
	}else
	{
	}
	return 0;
}
