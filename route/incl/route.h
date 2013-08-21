#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<assert.h>
#include<pthread.h>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <fcntl.h>
#include <bits/pthreadtypes.h>
#define LOG_SYS 1

int SysLog(int loglevel,char *format,...);
