#pragma once

#include <mutex> 

class ThreadPool
{
public:
	ThreadPool(); // ����������� �� ��������� - 0 �������
	ThreadPool(int maxThreadsCount);

	void setMaxThreads(int maxThreadsCount);

	bool tryGetThread();
	bool tryReturnThread();

private:
	int freeThreadsCount;
	int maxThreadsCount;
	std::mutex locker; // ��� ������������ ������ �/� �������
};
