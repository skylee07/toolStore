#pragma once
#include <vector>	// std::vector
#include <thread>
#include <queue>
#include <mutex>	// std::mutex

////////////////////////////////////////////////////////
#define	CLASSWORKBASE CWorkBase	// 工作类宏定义

#define RTN_FULL	1	// 队列已满
#define RTN_FAIL	-1	// 处理失败
#define	RTN_SUCC	0	// 处理成功

using namespace std;

/// CWorkBase 工作基类 //////////////////////////
/*
	CWorkBase 具体工作类的基类；基类提供执行接口函数DoWork()；
*/
class CWorkBase {
public:
	virtual int DoWork();
};

/// 锁类 ////////////////////////////////
class CAutoLock
{
public:
	CAutoLock(mutex &mtx, bool bTry = false);
	~CAutoLock();

private:
	mutex	*m_mtx;
};

/// 线程类  带任务类参数 模板 ////////////////////////////////
template <class CJob>
class CThread
{
public:
	CThread(CJob &ojob);

	int Run();
	int Stop();
	int Wait();

	// 线程函数
	static void FuncRun(void *oJob );

private:
	thread	m_thread;	// 线程句柄
	CJob	*m_pJob;	// 任务实例
};

/// 任务类 带工作类参数 模板 //////////////////////
template<class CWork>
class CJob
{
public:
	CJob();
	int		Work();
	int		AddTask( CWork &work );
	int		GetTaskSize();
	void	SetMaxTaskSize( int iMax );

	bool			m_bStop;			// 任务结束标记，用于线程结束

private:
	queue<CWork>	m_qTasks;
	int				m_iMaxSize;			// 最大任务数
	mutex			m_mutexTask;		// 任务临界区
};

/// 线程池 /////////////////////////////////////
template<class CWork>
class CThpool
{
public:
	CThpool();
	//	~CThpool();
	// 创建线程池
	int		Create( unsigned int iThreadSize = 20, unsigned int iMaxTask = 50 );
	// 停止线程池
	int		Stop();
	// 添加任务
	int		AddTask( CWork &work );

public:
	CJob<CWork>			m_oJob;				// 任务管理实例

private:
	int		m_iMaxPoolSize;		// 最大线程数
	int		m_iMaxTaskSize;		// 任务最大数
	vector<CThread<CJob<CWork>> *>		m_vecThreads;	// 线程列表
};

/// 测试函数 //////////////////////////////////
int FuncTest();