#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include"struct.h"

_ROUTECFG *getrtcfg(char *);
