#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	freeThreadsCount = 0;
	maxThreadsCount = 0;
}

ThreadPool::ThreadPool(int maxThreadsCount)
{
	freeThreadsCount = maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount;
}

void ThreadPool::setMaxThreads(int maxThreadsCount)
{
	locker.lock();
	freeThreadsCount = freeThreadsCount + maxThreadsCount - this->maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount;
	locker.unlock();
}

bool ThreadPool::tryGetThread()
{
	while (true)
	{
		if (locker.try_lock()) {
			bool hasFreeThread = freeThreadsCount > 0;
			if (hasFreeThread)
			{
				freeThreadsCount--;
			}
			locker.unlock();
			return hasFreeThread;
		}
	}

	return false;
}

bool ThreadPool::tryReturnThread()
{
	while (true)
	{
		if (locker.try_lock()) {
			bool hasFreeThread = freeThreadsCount < maxThreadsCount - 1;
			if (hasFreeThread)
			{
				freeThreadsCount++;
			}
			locker.unlock();
			return hasFreeThread;
		}
	}

	return false;
}