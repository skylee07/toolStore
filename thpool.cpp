#include <iostream>
#include "thpool.h"

using namespace std;

/// 自动锁 //////////////////////////////////////////////////
CAutoLock::CAutoLock(mutex &mtx, bool bTry /* = false */) { 
	m_mtx = &mtx;
	if (bTry) m_mtx->try_lock();
	else m_mtx->lock();
}
CAutoLock::~CAutoLock()
{
	m_mtx->unlock();
}

/// 线程类 ////////////////////////////////////////////////////
template<class CJob>
void CThread<CJob>::FuncRun( void *oJob)
{
	/* 线程函数 */
	printf("启动线程[%d]\n", this_thread::get_id());
	CJob * pJob = (CJob *)oJob;

	while (!pJob->m_bStop)
	{
		pJob->Work();
	}
}
template<class CJob>
CThread<CJob>::CThread(CJob &oJob){
	m_pJob = &oJob;
}
template<class CJob>
int CThread<CJob>::Run()
{
	// 创建线程
	m_thread = thread(FuncRun, m_pJob);
	return RTN_SUCC;
}
template<class CJob>
int CThread<CJob>::Stop()
{
	Wait();
	return RTN_SUCC;
}
template<class CJob>
int CThread<CJob>::Wait()
{
	m_thread.join();
	return RTN_SUCC;
}

/// 任务类 CJob /////////////////////////////////////////////////
template<class CWork>
CJob<CWork>::CJob() : m_bStop(false)
{

}
template<class CWork>
int CJob<CWork>::Work()
{
	if (0 == GetTaskSize()) {
		return RTN_SUCC;
	}

	CWork *work = NULL;
	{
		CAutoLock lock(m_mutexTask);
		work = &m_qTasks.front();
		m_qTasks.pop();
	}

	/// 工作类入口函数
	if ( NULL != work ) work->DoWork();

	return RTN_SUCC;
}

template<class CWork>
int CJob<CWork>::GetTaskSize() {
	CAutoLock lock(m_mutexTask);
	return m_qTasks.size();
}

template<class CWork>
void CJob<CWork>::SetMaxTaskSize(int iMax) { m_iMaxSize = iMax;  }

template<class CWork>
int CJob<CWork>::AddTask( CWork &work )
{
	if (m_iMaxSize <= GetTaskSize()) {
		printf("任务已满[%d]\n", m_iMaxSize);
		return RTN_FULL;
	}

	CAutoLock lock(m_mutexTask);
	m_qTasks.push(work);

	return RTN_SUCC;
}

/// 线程池 ////////////////////////////////////////////////////////
template<class CWork>
CThpool<CWork>::CThpool()
{
	m_iMaxPoolSize = 20;
	m_iMaxTaskSize = 50;
}
// 创建线程池
template<class CWork>
int CThpool<CWork>::Create(unsigned int iThreadSize /* = 20 */, unsigned int iMaxTask /* = 50 */ )
{
	// TODO: 在此处添加实现代码.
	if (0 != iThreadSize) m_iMaxPoolSize = iThreadSize;
	if (0 != iMaxTask) m_iMaxTaskSize = iMaxTask;

	// 设置任务实例的 任务数
	m_oJob.SetMaxTaskSize(iMaxTask);

	// 创建线程池
	for (unsigned int i = 0; i < iThreadSize; i++)
	{
		CThread<CJob<CWork>> *th = new CThread<CJob<CWork>>(m_oJob);
		th->Run();
		m_vecThreads.push_back(th);
	}

	// 等待所有进程都启动
	int num = 100000;
	while (num--);

	return RTN_SUCC;
}
template<class CWork>
int CThpool<CWork>::Stop()
{
	// 检查任务是否执行完
	while (0 != m_oJob.GetTaskSize())
	{
		printf("等待结束，剩余任务数[%d]\n", m_oJob.GetTaskSize());
	}

	// 设置任务管理停止
	m_oJob.m_bStop = true;

	// 关闭线程池
	for ( unsigned int i = 0; i < m_vecThreads.size(); i++)
	{
		m_vecThreads[i]->Stop();
		delete m_vecThreads[i];
	}
	m_vecThreads.clear();

	printf("线程池已关闭\n");
	return RTN_SUCC;
}
template<class CWork>
int CThpool<CWork>::AddTask(CWork &work) {
	return m_oJob.AddTask(work);
}


/// 工作基类 ///////////////////////////////////////////////////
int CWorkBase::DoWork() {
	printf("[%d] 执行工作 CWorkBase::DoWork()\n", this_thread::get_id() );
	return RTN_SUCC;
}

//////////////////////////////////////////////////////
int FuncTest()
{
	CThpool<CLASSWORKBASE> thP;

	thP.Create(3, 10);

	CLASSWORKBASE	work;
	for (int i = 20; i > 0; i--) {
		if (RTN_FULL == thP.AddTask(work)) i++;
	}
	//Sleep(1000);
	thP.Stop();

	return RTN_SUCC;
}