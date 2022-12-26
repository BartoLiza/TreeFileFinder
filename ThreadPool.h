#pragma once

#include <mutex>

class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(int maxThreadsCount);

	void setMaxThreads(int maxThreadsCount);

	bool tryGetThread();
	bool tryReturnThread();

private:
	int freeThreadsCount;
	int maxThreadsCount;
	std::mutex locker;
};