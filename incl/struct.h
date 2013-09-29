/** the shm struct of the platform **/

/* the flow struct
 * author:LiLei
 */

typedef struct FLOW
{
	int	flowid;//流程序号
	char	flow_mark[60];//流程说明
	char	func_name[20];//流程函数名称
	char	dl_name[10];//动态库名称
	char	flow_type[2];//流程类型
	char	errflow[60];//出错流程
	char	timeoutflow[60];//超时流程
}_flow;

typedef struct FLOWINDEX
{
	char	flow_name[60];//流程名称
	_flow	*detail_flow;
}_flowindex;


/*the data struct
 * author:LiLei
 */
typedef struct DATA
{
	char	varname[10];
	char	value[100];
}_data;

/*the main tran shm struct 
 * author:LiLei
 */

typedef struct TRANSHM
{
	pthread_t s_pid;//发起渠道线程号
	pthread_t a_pid;//核心处理线程号
	pthread_t r_pid;//接收渠道线程号
	char	send_chnl[5];//发起渠道标志
	char	serv_name[5];//核心服务名称
	char	recv_chnl[5];//接收渠道标志
	char	chnl_data[4096];//渠道申请数据
	char	senv_data[4096];//核心申请数据
	_flow 	*do_flow;		//当前处理流程指针
	_data	*do_data;		//当前变量值数据区
	char	return_serv[4096];//返回核心数据
	char	return_chnl[4096];//返回渠道数据
	time_t	stime;//开始时间
	time_t	dtime;//处理时间
	time_t	etime;//结束时间
	char	cur_stat[2];//当前状态
	char	require_serv[3];//所需服务类型
	char	xxx1[1];//备用1
	char	xxx2[1];//备用1
	char	xxx3[1];//备用1
	char	xxx4[1];//备用1
}_transhm;

/* the server shm index struct
 * author:LiLei
 */

typedef struct SERVERSHM
{
	pid_t	serverpid;//目前提供服务的PID
	char	servtype[2];//C 渠道 S 核心服务
	char	require_serv[3];//提供服务的服务类型
	char	chnlid[5];//渠道标志
	int		cnt;//处理能力 为0 时,不对外提供服务
}_servershm;
