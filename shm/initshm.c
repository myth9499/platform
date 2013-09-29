#include "platform.h"
#include "struct.h"

int main(int argc,char *argv[])
{
    int shmid = 0;
    key_t key;
	int	maxtran = 10;//需要从配置文件获取最大支持交易共享内存
	int maxserver = 50;//服务和渠道共计最大数
	int	maxflowindex = 10;//需要从配置文件获取最大支持交易共享内存
	int	maxdetailflow = 1000;//需要从配置文件获取最大支持交易共享内存

	/** 需要修改为从配置文件读取 **/
    key = ftok("/item/platform/route/etc/SHMKEY",11);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,maxtran*sizeof(_transhm),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);

    key = ftok("/item/platform/route/etc/SHMKEY",12);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,maxflowindex*sizeof(_flowindex),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);

    key = ftok("/item/platform/route/etc/SHMKEY",13);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,maxdetailflow*sizeof(_flow),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);

    key = ftok("/item/platform/route/etc/SHMKEY",14);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,maxserver*sizeof(_servershm),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
    return 0;
}
