#include "ResultsObject.h"

void ResultsObject::putResult(string path)
{
	readWriteMutex.lock();
	results.push_back(path);
	readWriteMutex.unlock();
}

vector<string>* ResultsObject::getResults()
{
	readWriteMutex.lock();

	vector<string>* res = new vector<string>();
	for (int i = 0; i < results.size(); i++)
	{
		res->push_back(results[i]);
	}
	readWriteMutex.unlock();

	return res;
}