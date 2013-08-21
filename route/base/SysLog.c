#include "route.h"

int SysLog(int loglevel,char *format,...)
{
    FILE *fp = NULL;
    va_list argptr;
    int ret;
    char *log_path=NULL;
    va_start(argptr,format);

    log_path = (char *)malloc(sizeof(char)*2048);
    if(log_path==NULL)
    {
        perror("Malloc error");
        return -1;
    }
	if(loglevel==1)
	{
    	sprintf(log_path,"/item/platform/route/base/sys.log");
	}else
	{
    	sprintf(log_path,"/item/platform/route/base/log_%ld.log",(long)pthread_self());
	}
	printf("log path is [%s]\n",log_path);
    fp = fopen(log_path,"a");
    if(fp == NULL)
    {
        perror("file open error");
        return -1;
    }
    ret = vfprintf(fp,format,argptr);
    fclose(fp);
    va_end(argptr);
    return (ret);
}
