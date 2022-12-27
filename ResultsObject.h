#pragma once

#include <string>
#include <vector>
#include <mutex>

using namespace std;

class ResultsObject
{
public:
	void putResult(string path);
	vector<string>* getResults();

private:
	vector<string> results;
	mutex readWriteMutex;
};

static ResultsObject results; // статическая переменная результатов поиска, которая доступна из любой точки программы 