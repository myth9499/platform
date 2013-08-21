/** the shm struct of the platform **/

/*the main tran shm struct 
 * author:LiLei
 */

typedef struct TRANSHM
{
	pthread_t s_pid;//发起渠道线程号
	pthread_t a_pid;//核心处理线程号
	pthread_t r_pid;//接收渠道线程号
	int 	chnlid;//核心发送到渠道的渠道ID
	char	type[2];//所需服务类型,给各个交易分服务处理
	char	s_data[9];//发起日期
	char	s_time[9];//发起时间
	char	e_data[9];//结束日期
	char	e_time[9];//结束时间
	char	in_data[1024];//内部控制数据
	char	tran_data[4096];//实际交易数据
	char	end_data[4096];//核心处理后需要发送到渠道的数据
	char	storetype[2];//存储类型 F-文件名 B-字节存储
	char	stat[2];//交易状态  W-待处理 A-处理中 E-处理结束 T-超时
	char	sync[2];//sync stat Y-sync  N-NO SYNC
}_transhm;
