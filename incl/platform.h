/** the public include file **/
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <bits/pthreadtypes.h>

/** system define errno **/
#define SYS_ERR -1
#define SYS_OK  0
