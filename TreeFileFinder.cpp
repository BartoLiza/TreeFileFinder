// TreeFileFinder.cpp: определяет точку входа для приложения.
//

#include "TreeFileFinder.h"


int main(int argc, char** argv) {

	vector<string> args;

	for (int i = 0; i < argc; i++)
	{
		args.push_back(string(argv[i]));
	}

	if (args.size() <= 1)
	{
		return -1;
	}

	string fileName = args[1];
	string basePath = DEFAULT_BASE_PATH;

	int threadsCount = DEFAULT_THREADS_COUNT;

	for (int i = 2; i < args.size(); i++)
	{
		if (args[i] == PATH_PARAM && i + 1 < args.size())
		{
			basePath = args[i + 1];
		}
		else if (args[i] == THREADS_COUNT_PARAM && i + 1 < args.size())
		{
			string threadsCountStr = args[i + 1];
			if (sscanf(args[i + 1].c_str(), "%d", &threadsCount) == 1)
			{
			}
		}
	}

	if (basePath.empty())
	{
		basePath = DEFAULT_BASE_PATH;
	}

	if (!fs::is_directory(basePath))
	{
		cout << "Can`t use this path" << basePath << endl;
		return -2;
	}

	DirObject baseDirObj(basePath);

	cout << "Threads count " << threadsCount << " threads..." << endl;

	cout << "Collecting files and directories " << threadsCount << " threads..." << endl;

	collect(&baseDirObj);

	cout << "Collected files and directories!" << endl;

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
