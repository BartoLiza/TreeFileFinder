#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	freeThreadsCount = 0;
	maxThreadsCount = 0;
}

ThreadPool::ThreadPool(int maxThreadsCount)
{
	freeThreadsCount = maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount; //this-> - чтобы обратиться к элементу класса
}

void ThreadPool::setMaxThreads(int maxThreadsCount) // если во время работы программы изенится max кол-во потоков 
{
	locker.lock(); 
	freeThreadsCount = freeThreadsCount + maxThreadsCount - this->maxThreadsCount;
	this->maxThreadsCount = maxThreadsCount;
	locker.unlock();
}

bool ThreadPool::tryGetThread() // для проверки свободных потоков для использования
{
	while (true)
	{
		if (locker.try_lock()) { // try_lock() пытаемся заблокировать mutex
			bool hasFreeThread = freeThreadsCount > 0; // true, если есть свободные потоки 
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

bool ThreadPool::tryReturnThread() // для возврата занятого потока в свободный
{
	while (true)
	{
		if (locker.try_lock()) {
			bool hasFreeThread = freeThreadsCount < maxThreadsCount - 1; // контроль свободных потоков (не больше max)
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