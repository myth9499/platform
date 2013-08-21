/*
 * 共享内存区存储数据
 * 李磊 2013.5.6
 */

#define MAXTRAN 9999
typedef struct TRANCFG
{
	char tranid[4+1];
	char tranmask[60+1];
	char servip[15+1];
	char servport[6+1];
	int  timeout;
}_ROUTECFG;

typedef struct SHMMON
{
	char tranid[4+1];
	char tranmask[60+1];
	long rightcnt;
	long errcnt;
	long timeoutcnt;
}_SHMMON;
