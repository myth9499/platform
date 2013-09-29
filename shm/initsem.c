#include "platform.h"
#include "struct.h"

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};
int main(int argc,char *argv[])
{
    key_t key;
	int semid=0;

    key = ftok("/item/platform/route/etc/SHMKEY",21);
    if(key == (key_t)-1)
    {
        printf("获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }

    printf("获取主键成功[%s][%d]",__FILE__,__LINE__);
	if((semid=semget(key,1,IPC_CREAT|0666))<0)
	{
		perror("fail to semget");
		return -1;
	}

	union semun mysemun;
	mysemun.val = 0;
	if(semctl(semid,0,SETVAL,mysemun)<0)
	{
		perror("fail to semctl");
		return -1;
	}
	printf("信号灯初始化OK\n");
}
