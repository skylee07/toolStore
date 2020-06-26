#pragma once
#include <vector>	// std::vector
#include <thread>
#include <queue>
#include <mutex>	// std::mutex

////////////////////////////////////////////////////////
#define	CLASSWORKBASE CWorkBase	// ������궨��

#define RTN_FULL	1	// ��������
#define RTN_FAIL	-1	// ����ʧ��
#define	RTN_SUCC	0	// ����ɹ�

using namespace std;

/// CWorkBase �������� //////////////////////////
/*
	CWorkBase ���幤����Ļ��ࣻ�����ṩִ�нӿں���DoWork()��
*/
class CWorkBase {
public:
	virtual int DoWork();
};

/// ���� ////////////////////////////////
class CAutoLock
{
public:
	CAutoLock(mutex &mtx, bool bTry = false);
	~CAutoLock();

private:
	mutex	*m_mtx;
};

/// �߳���  ����������� ģ�� ////////////////////////////////
template <class CJob>
class CThread
{
public:
	CThread(CJob &ojob);

	int Run();
	int Stop();
	int Wait();

	// �̺߳���
	static void FuncRun(void *oJob );

private:
	thread	m_thread;	// �߳̾��
	CJob	*m_pJob;	// ����ʵ��
};

/// ������ ����������� ģ�� //////////////////////
template<class CWork>
class CJob
{
public:
	CJob();
	int		Work();
	int		AddTask( CWork &work );
	int		GetTaskSize();
	void	SetMaxTaskSize( int iMax );

	bool			m_bStop;			// ���������ǣ������߳̽���

private:
	queue<CWork>	m_qTasks;
	int				m_iMaxSize;			// ���������
	mutex			m_mutexTask;		// �����ٽ���
};

/// �̳߳� /////////////////////////////////////
template<class CWork>
class CThpool
{
public:
	CThpool();
	//	~CThpool();
	// �����̳߳�
	int		Create( unsigned int iThreadSize = 20, unsigned int iMaxTask = 50 );
	// ֹͣ�̳߳�
	int		Stop();
	// �������
	int		AddTask( CWork &work );

public:
	CJob<CWork>			m_oJob;				// �������ʵ��

private:
	int		m_iMaxPoolSize;		// ����߳���
	int		m_iMaxTaskSize;		// ���������
	vector<CThread<CJob<CWork>> *>		m_vecThreads;	// �߳��б�
};

/// ���Ժ��� //////////////////////////////////
int FuncTest();