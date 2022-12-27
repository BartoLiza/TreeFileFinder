#include "ResultsObject.h"

void ResultsObject::putResult(string path)
{
	readWriteMutex.lock(); // ��������� ������ ��� ������ �� ������ �������
	this->results.push_back(path); //  ���������� � ������� ������������ ����� ��������� ����
	readWriteMutex.unlock();
}

vector<string>* ResultsObject::getResults()
{
	readWriteMutex.lock();

	vector<string>* res = new vector<string>();
	for (int i = 0; i < this->results.size(); i++) // �������� ���������� ������� results ��� ������ ���������� (���������������)
	{
		res->push_back(this->results[i]);
	}
	readWriteMutex.unlock();

	return res;
}