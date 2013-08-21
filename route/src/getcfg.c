#include "struct.h"
#include "route.h"

/**
 * 获取配置参数，确认当前系统是从数据库读取还是文件读取
 * 作者：李磊
 * 时间：2013.5.3
 * 0-文件读取 1-mysql读取
 */
int getsyscfg(void)
{
    FILE *fp=NULL;
    char str[256];
    char *fstr=NULL;

    memset(str,0,sizeof(str));
    fp  = fopen("/item/platform/route/cfg/sys.cfg","r");
    if(fp == NULL)
    {
        perror("OPEN FILE ERROR");
        return -1;
    }
    printf("配置文件打开成功\n");
    while(fgets(str,sizeof(str),fp)!=NULL)
    {
        if(str[0]=='#')
        {
//            printf("注释行,跳过解析\n");
            memset(str,0,sizeof(str));
            continue;
        }
        str[strlen(str)-1]='\0';
//       printf("str is [%s]\n",str);
        if((fstr = strstr(str,"CFGTYPE"))==NULL)
        {
            //          printf("改行不存在配置\n");
            memset(str,0,sizeof(str));
            continue;
        }
        //     printf("the fstr is[%s]\n",fstr);
        if(!strcmp(fstr+(strlen("CFGTYPE")+1),"FILE"))
        {
            printf("系统从文件获取配置\n");
            fclose(fp);
            return 0;
        }
        if(!strcmp(fstr+(strlen("CFGTYPE")+1),"MYSQL"))
        {
            printf("系统从MYSQL数据库获取配置\n");
            fclose(fp);
            return 1;
        }
        memset(str,0,sizeof(str));
    }
    printf("配置文件错误，缺少CFGTYPE项\n");
    fclose(fp);
    return -1;
}
/*
 * 从文件配置信息读取系统配置
 * 作者：李磊
 * 2013.5.3
 */
int getfilecfg(void)
{
    FILE *fp=NULL;
    FILE *fp2=NULL;
    char str[256];
	char tran_id[5];
	char tran_mask[61];
    char detailstr[256];
    char *fstr=NULL;
    int shmid = 0;
    int shmid2 = 0;
    key_t key ;
    _ROUTECFG *rtcfg = NULL;
    key_t key2 ;
    _SHMMON *shmmon = NULL;


    key = ftok("/item/platform/route/etc/SHMKEY",11);
    if(key == (key_t)-1)
    {
        perror("ftok key error");
        return -1;
    }
    printf("ftok ok\n");
    shmid = shmget(key,MAXTRAN*sizeof(_ROUTECFG),IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid);

    key2 = ftok("/item/platform/route/etc/SHMKEY",12);
    if(key2 == (key_t)-1)
    {
        perror("ftok key error");
        return -1;
    }
    printf("ftok ok\n");
    shmid2 = shmget(key2,MAXTRAN*sizeof(_SHMMON),IPC_EXCL|S_IRUSR|S_IWUSR);
    if(shmid2 == -1)
    {
        perror("shm get error");
        return -1;
    }
    printf("shmget ok shmid is [%d]\n",shmid2);

   	rtcfg = (_ROUTECFG *)shmat(shmid,NULL,0);
    if(rtcfg == (void *)-1)
    {
        perror("shmat error");
        return -1;
    }
    printf("shmat ok\n");

    shmmon = (_SHMMON *)shmat(shmid2,NULL,0);
    if(shmmon == (void *)-1)
    {
        perror("shmat error");
        return -1;
    }
    printf("shmat ok\n");

    memset(str,0,sizeof(str));
    memset(detailstr,0,sizeof(detailstr));
    fp  = fopen("/item/platform/route/cfg/sys.cfg","r");
    if(fp == NULL)
    {
        perror("OPEN FILE ERROR");
        return -1;
    }
    printf("配置文件打开成功\n");
    while(fgets(str,sizeof(str),fp)!=NULL)
    {
        if(str[0]=='#')
        {
            //        printf("注释行,跳过解析\n");
            memset(str,0,sizeof(str));
            continue;
        }
        str[strlen(str)-1]='\0';
        if(!strcmp(str,"[/FILE]"))
        {
            printf("所有详细配置文件读取结束\n");
            fclose(fp);
            fclose(fp2);
            break;
        }
        //   printf("str is [%s]\n",str);
        if((fstr = strstr(str,"FILEPATH"))==NULL)
        {
            //      printf("该行不存在配置\n");
            memset(str,0,sizeof(str));
            continue;
        }
        // printf("the fstr is[%s]\n",fstr);
        if((fp2=fopen((fstr+(strlen("FILEPATH")+1)),"r"))==NULL)
        {
            printf("打开详细配置文件失败\n");
            fclose(fp);
            return -1;
        } else
        {
            printf("打开详细配置文件成功\n");
            while(fgets(detailstr,sizeof(detailstr),fp2)!=NULL)
            {
                detailstr[strlen(detailstr)-1]='\0';
                //        printf("detailstr is [%s]\n",detailstr);
                if(detailstr[0]=='#')
                {
                    memset(detailstr,0,sizeof(detailstr));
                    continue;
                } else if(detailstr[0]=='*')
                {
                    printf("开始处理[%s]\a\n",detailstr+1);
                    memset(detailstr,0,sizeof(detailstr));
                    continue;
                }
                {
					memset(tran_id,0,sizeof(tran_id));
					strcpy(tran_id,strtok(detailstr,"|"));
                    //printf("交易码[%s]\n",strtok(detailstr,"|"));
                    //strcpy(rtcfg->tranid,strtok(detailstr,"|"));
                    strcpy(rtcfg->tranid,tran_id);
                    strcpy(shmmon->tranid,tran_id);
                    //printf("说明[%s]\n",strtok(NULL,"|"));
					memset(tran_mask,0,sizeof(tran_mask));
					strcpy(tran_mask,strtok(NULL,"|"));
                    //strcpy(rtcfg->tranmask,strtok(NULL,"|"));
                    strcpy(rtcfg->tranmask,tran_mask);
                    strcpy(shmmon->tranmask,tran_mask);
                    //printf("对方地址[%s]\n",strtok(NULL,"|"));
                    strcpy(rtcfg->servip,strtok(NULL,"|"));
                    //printf("对方端口[%s]\n",strtok(NULL,"|"));
                    strcpy(rtcfg->servport,strtok(NULL,"|"));
                    //printf("超时时间[%s]\n",strtok(NULL,"|"));
                    rtcfg->timeout = atoi(strtok(NULL,"|"));
                    rtcfg++;
                    shmmon++;
                }
                memset(detailstr,0,sizeof(detailstr));
            }
        }
        memset(str,0,sizeof(str));
    }
    strcpy(rtcfg->tranid,"END");
    strcpy(shmmon->tranid,"END");
    //printf("配置文件错误，缺少CFGTYPE项\n");
    //fclose(fp);
    //fclose(fp2);
    return -1;
}
int main(int argc,char *argv[])
{
    printf("[%d]\n",getsyscfg());
    if(getsyscfg()==0)
    {
        getfilecfg();
    }
    return 0;
}
