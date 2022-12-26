#pragma once

#include <mutex> 

class ThreadPool
{
public:
	ThreadPool(); // конструктор по умолчанию - 0 потоков
	ThreadPool(int maxThreadsCount);

	void setMaxThreads(int maxThreadsCount);

	bool tryGetThread();
	bool tryReturnThread();

private:
	int freeThreadsCount;
	int maxThreadsCount;
	std::mutex locker; // для синхрониации данных м/д потоков
};
