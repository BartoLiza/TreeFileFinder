// TreeFileFinder.cpp: определяет точку входа для приложения.
//

#include "TreeFileFinder.h"
#include "ThreadPool.h"


int main(int argc, char** argv) {

	vector<string> args;
	/* нулевой параметр - название программы, потом - название файла, который ищем + допю параметры --path/ --num_threads - либо путь, либо кол-во потоков. 
	путь - папка с которого начинаем строить дерево*/
	for (int i = 0; i < argc; i++)
	{
		args.push_back(string(argv[i])); 
	}
	
	if (args.size() <= 1)
	{
		return -1;
	}

	string fileName = args[1]; // название файла
	string basePath = DEFAULT_BASE_PATH; // базовый путь по умолчанию (С)
	int threadsCount = DEFAULT_THREADS_COUNT;//  по умолчанию (10)
	
	for (int i = 2; i < args.size(); i++)
	{
		if (args[i] == PATH_PARAM && i + 1 < args.size()) // если нашли --path и это не последний элемент вектора
		{
			basePath = args[i + 1];
		}
		else if (args[i] == THREADS_COUNT_PARAM && i + 1 < args.size())// если нашли --num_threars и это не последний элемент вектора
		{
			string threadsCountStr = args[i + 1];
			if (sscanf(args[i + 1].c_str(), "%d", &threadsCount) == 1) // преобразование строки (10) в число
			{
				cout << "Can`t convert --nem_threads value { " << args[i + 1] << " } to int" << endl;
				cin.get();
				return -3;
			}
		}
	}
	
	if (basePath.empty()) // --path  --num_threats 10
	{
		basePath = DEFAULT_BASE_PATH;
	}

	if (!fs::is_directory(basePath)) // проверка, что наш путь не директория (fs - обращение к файлам в системе)
	{
		cout << "Can`t use this path" << basePath << endl;
		return -2;
	}

	cout << "Threads count " << threadsCount << " threads..." << endl; 

	ThreadPool* threadPool = new ThreadPool(threadsCount);

	DirObject baseDirObj(basePath);

	cout << "Collecting files and directories" << endl;

	collect(&baseDirObj);

	cout << "Collected files and directories!" << endl;

	vector<string>* result = find(fileName, &baseDirObj, threadPool);

	if (!result->empty())
	{
		for (string resPath : *result)
		{
			cout << "Found file: " << resPath << endl;
		}
	}
	else
	{
		cout << "Can`t find this file: " << fileName << endl;
	}

	cout << "End" << endl;
	cin.get();
	return 0;
}

bool collect(DirObject* baseDirObj)
{
	if (baseDirObj->checkDir())
	{
		baseDirObj->collectChildsRecursive();
		return true;
	}
	return false;
}

vector<string>* find(string fileName, DirObject* baseDirObj, ThreadPool* threadPool)
{
	return baseDirObj->findFile(fileName, threadPool);
}
