// TreeFileFinder.cpp: определяет точку входа для приложения.
//

#include "TreeFileFinder.h"
#include "ThreadPool.h"
#include "ResultsObject.h"

int main(int argc, char** argv) {

	vector<string>* args = new vector<string>();

	/* нулевой параметр - название программы, потом - название файла, который ищем + допю параметры --path/ --num_threads - либо путь, либо кол-во потоков.
	путь - папка с которого начинаем строить дерево*/
	for (int i = 0; i < argc; i++)
	{
		args->push_back(string(argv[i]));
	}

	if (args->size() <= 1)
	{
		cout << "Missing file name parameter" << endl;
		cin.get();
		return -1;
	}

	string* fileName = new string((*args)[1]);

	if (!(*fileName != THREADS_COUNT_PARAM && *fileName != PATH_PARAM && *fileName != ""))
	{
		std::cout << "Name of file is not set" << endl;
		return -1;
	}

	string* basePath = new string(DEFAULT_BASE_PATH); // базовый путь по умолчанию (С)
	int* threadsCount = new int(DEFAULT_THREADS_COUNT);//  по умолчанию (10)

	for (int i = 2; i < args->size(); i++)
	{
		if ((*args)[i] == PATH_PARAM && i + 1 < args->size()) // если нашли --path и это не последний элемент вектора
		{
			i++;
			*basePath = (*args)[i];
		}
		else if ((*args)[i] == THREADS_COUNT_PARAM && i + 1 < args->size())// если нашли --num_threars и это не последний элемент вектора
		{
			i++;
			if (sscanf((*args)[i].c_str(), "%d", threadsCount) != 1) // преобразование строки (10) в число
			{
				cout << "Can`t convert --nem_threads value { " << (*args)[i] << " } to int" << endl;
				cin.get();
				return -3;
			}
		}
	}

	args->clear();
	delete(args); // очистка памяти занятой вектором args

	if (basePath->empty()) // --path  --num_threats 10
	{
		*basePath = DEFAULT_BASE_PATH;
	}

	if (!fs::is_directory(*basePath)) // проверка, что наш путь не директория (fs - обращение к файлам в системе)
	{
		cout << "Can`t use this path" << *basePath << endl;
		return -2;
	}

	cout << "Threads count " << *threadsCount << " threads..." << endl;



	ThreadPool* threadPool = new ThreadPool((*threadsCount)-1); //создаем пул потоков и задаем max кол-во
	delete(threadsCount); // очистка памяти занятой переменной threadsCount

	DirObject* baseDirObj = new DirObject(*basePath); // создаем главный узел дерева
	delete(basePath); // очистка памяти занятой переменной basePath

	cout << "Collecting files and directories" << endl;

	collect(baseDirObj); // сбор узлов дерева

	cout << "Collected files and directories!" << endl;

	ResultsObject* resultsObj = new ResultsObject();

	find(*fileName, baseDirObj, threadPool, resultsObj);

	delete(baseDirObj); // очистка памяти занятой главным узлом дерева
	delete(threadPool); // очистка памяти занятой пулом потоков

	vector<string>* result = resultsObj->getResults();

	if (!result->empty())
	{
		for (string resPath : *result) //перебираем список путей надйнных файлов
		{
			cout << "Found file: " << resPath << endl;
		}
	}
	else
	{
		cout << "Can`t find this file: " << *fileName << endl;
	}

	delete(result); // очистка памяти занятой вектором результатов
	delete(resultsObj); // очистка памяти занятой объемом результатов
	delete(fileName); // очистка памяти занятой переменной fileName

	cout << "End" << endl;

	cin.get();
	return 0;
}

void collect(DirObject* baseDirObj)
{
	if (baseDirObj->checkDir()) // если доступна
	{
		baseDirObj->collectChildsRecursive(); // собираем дерево рекурсивно
	}
}

void find(string fileName, DirObject* baseDirObj, ThreadPool* threadPool, ResultsObject* resultsObj)
{
	return baseDirObj->findFile(fileName, threadPool, resultsObj); // ищем в главном узле дерева файл рекурсивно
}
