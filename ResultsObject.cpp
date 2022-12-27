#include "ResultsObject.h"

void ResultsObject::putResult(string path)
{
	readWriteMutex.lock(); // блокируем запись для записи из разных потоков
	this->results.push_back(path); //  добавление к вектору результатовв новый найденный путь
	readWriteMutex.unlock();
}

vector<string>* ResultsObject::getResults()
{
	readWriteMutex.lock();

	vector<string>* res = new vector<string>();
	for (int i = 0; i < this->results.size(); i++) // копируем содержимое вектора results для вывода результата (потокобезопасно)
	{
		res->push_back(this->results[i]);
	}
	readWriteMutex.unlock();

	return res;
}