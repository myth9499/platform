#include "struct.h"
#include "route.h"

int main(int argc,char *argv[])
{
    int shmid = 0;
    key_t key;
    _SHMMON *shmmon =NULL;
    _SHMMON *tmp =NULL;

    key = ftok("/item/platform/route/etc/SHMKEY",12);
    if(key == (key_t)-1)
    {
        perror("ftok key error");
        return -1;
    }
    printf("ftok ok\n");
    shmid = shmget(key,MAXTRAN*sizeof(_SHMMON),IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
    shmmon = shmat(shmid,NULL,0);
    if(shmmon ==(void *)-1)
    {
        perror("shmat error");
        return -1;
    }
SHMMON_CONTINUE:
	/**
    shmmon = shmat(shmid,NULL,0);
    if(shmmon ==(void *)-1)
    {
        perror("shmat error");
        return -1;
    }
	**/
	tmp = shmmon;
    while(strcmp(tmp->tranid,"END"))
    {
        printf("交易码[%s]\t 交易说明[%s]\t 正确笔数[%ld]\t 失败笔数[%ld]\t 超时笔数[%ld]\n",tmp->tranid,tmp->tranmask,tmp->rightcnt,tmp->errcnt,tmp->timeoutcnt);
        tmp++;
    }
	printf("------------------------------------------------\n");
	sleep (1);
	goto SHMMON_CONTINUE;
    return 0;
}
