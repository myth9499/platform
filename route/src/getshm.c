#include "struct.h"
#include "route.h"

_ROUTECFG * getrtcfg(char *tranid)
{
    int shmid = 0;
    key_t key;
    _ROUTECFG *rtcfg =NULL;

    key = ftok("/item/platform/route/etc/SHMKEY",11);
    if(key == (key_t)-1)
    {
        perror("ftok key error");
        return NULL;
    }
    printf("ftok ok\n");
    shmid = shmget(key,MAXTRAN*sizeof(_ROUTECFG),IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return NULL;
    }
    printf("shmget ok shmid is [%d]\n",shmid);
    rtcfg = shmat(shmid,NULL,0);
    if(rtcfg ==(void *)-1)
    {
        perror("shmat error");
        return NULL;
    }
    while(strcmp(rtcfg->tranid,tranid))
    {
        printf("!!![%s]\t tanid[%s] \t \n",rtcfg->tranid,tranid);
        if(!strcmp(rtcfg->tranid,"END"))
            return NULL;
        rtcfg++;
    }
    return rtcfg;
}
