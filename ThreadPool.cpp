#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	freeThreadsCount = 0;
	maxThreadsCount = 0;
}

ThreadPool::ThreadPool(int maxThreadsCount)
{
	freeThreadsCount = maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount; //this-> - ����� ���������� � �������� ������
}

void ThreadPool::setMaxThreads(int maxThreadsCount) // ���� �� ����� ������ ��������� �������� max ���-�� ������� 
{
	locker.lock();
	freeThreadsCount = freeThreadsCount + maxThreadsCount - this->maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount;
	locker.unlock();
}

bool ThreadPool::tryGetThread() // ��� �������� ��������� ������� ��� �������������
{
	locker.lock();  // lock() ��������� mutex

	bool hasFreeThread = freeThreadsCount > 0; // true, ���� ���� ��������� ������ 
	if (hasFreeThread)
	{
		freeThreadsCount--;
	}

	locker.unlock();

	return hasFreeThread;
}

bool ThreadPool::tryReturnThread() // ��� �������� �������� ������ � ���������
{
	locker.lock();

	bool hasFreeThread = freeThreadsCount < maxThreadsCount - 1; // �������� ��������� ������� (�� ������ max)
	if (hasFreeThread)
	{
		freeThreadsCount++;
	}

	locker.unlock();

	return hasFreeThread;
}