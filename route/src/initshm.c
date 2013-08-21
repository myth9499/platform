#include "struct.h"
#include "route.h"

int main(int argc,char *argv[])
{
    int shmid = 0;
    key_t key;

    key = ftok("/item/platform/route/etc/SHMKEY",11);
    if(key == (key_t)-1)
    {
        SysLog(LOG_SYS,"获取主键错误[%s][%d][%s]",__FILE__,__LINE__,strerror(errno));
        return -1;
    }
    SysLog(LOG_SYS,"获取主键成功[%s][%d]",__FILE__,__LINE__);
    shmid = shmget(key,MAXTRAN*sizeof(_ROUTECFG),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);

    key = ftok("/item/platform/route/etc/SHMKEY",12);
    if(key == (key_t)-1)
    {
        perror("ftok key error");
        return -1;
    }
    printf("ftok ok\n");
    shmid = shmget(key,MAXTRAN*sizeof(_SHMMON),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
    return 0;
}
