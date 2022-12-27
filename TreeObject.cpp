#include "TreeObject.h"
#include "ThreadPool.h"

TreeObject::TreeObject(string fullPath) : TreeObject((fs::path)fullPath)
{
}

TreeObject::TreeObject(fs::path fullPath) : TreeObject(fullPath, fullPath.filename().string()) // filename() - ���������� ��� � �����������
{
}

TreeObject::TreeObject(fs::path fullPath, string name)
{
	this->fullPath = fullPath;
	this->name = name;
}

FileObject::FileObject(string fullPath) : TreeObject(fullPath) {} //���������� ������������ �����������
FileObject::FileObject(fs::path fullPath) : TreeObject(fullPath) {}

DirObject::DirObject(string fullPath) : TreeObject(fullPath) {}
DirObject::DirObject(fs::path fullPath) : TreeObject(fullPath) {}

bool DirObject::checkDir()
{
	try {
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath }; // ��������� ����������� ����������� �����
		return true; // ��������!
	}
	catch (exception)
	{
		return false; // �� ��������! (��������� �����, ������� �� �� ����� ���������)
	}
}

void DirObject::collectChilds()
{
	try { // �������� ����� �� ����������� ��������
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath }; //  directory_iterator - ��������� �� ���� ������ �����

		for (fs::directory_iterator iter = fs::begin(dirIter), endIter = fs::end(dirIter); iter != endIter; iter++) // ���� ���������� �� ���������� �����
		{
			try {
				fs::directory_entry dir_entry = *iter; // directory_entry - ����� ��� ���� ������ �����, ����� ������� �������
				if (dir_entry.is_directory()) // �������� �� ������� ������� ������?
				{
					DirObject* dir = new DirObject(dir_entry.path()); // ������� ����� ������� ������ (������ �����) 
					if (dir->checkDir()) // ���� ��������
					{
						dirs.push_back(dir); // ������ ����� � ������
					}
				}
				else
				{
					FileObject* file = new FileObject(dir_entry.path());// ������� ����� ������� ������ (������ �����) 
					files.push_back(file);
				}
			}
			catch (exception)
			{
				//cout << "Can`t use this path" << fullPath << endl;
			}
		}
	}
	catch (exception)
	{
		//cout << "Can`t open path" << fullPath << endl;
	}
}

void DirObject::collectChildsRecursive() // 
{
	collectChilds();
	for (DirObject* dir : dirs) // ���������� ��� �������� �������� dirs
	{
		dir->collectChildsRecursive();
	}
}

void DirObject::findFile(const string fileName, ThreadPool* threadPool, ResultsObject* resultsObj)
{
	threadPool->tryGetThread(); // ��������� ������� �����
	findFileThread(this, fileName, threadPool, resultsObj, false); // �������� ����� � ������� ������
}

void findFileThread(DirObject* dir, const string fileName, ThreadPool* threadPool, ResultsObject* resultsObj, bool isNewThread) // ������� ������ ������
{
	for (FileObject* file : dir->files) // ���������� ����� � ������� �����
	{
		if (file->name == fileName) // ���� ������� ���������� ����� �����
		{
			resultsObj->putResult(file->fullPath.string()); //������ ���� ��������� ����� � ����������
		}
	}

	if (dir->dirs.size() > 0) // ���� ���� ��������� �����
	{
		vector<DirObject*> dirsToFindInThread; // ������ � ������� �� ������� ������� �������

		vector<thread> childThreads;// ������ � �������� � ������ ���� 


		for (int i = 0; i + 1 < dir->dirs.size(); i++)
		{
			if (threadPool->tryGetThread())
			{
				dirsToFindInThread.push_back(dir->dirs[i]); // ��������� ����� � ������ �� ������� ������� ������
			}
			else
			{

				for (DirObject* nextDir : dirsToFindInThread) // ���������� ������ ����� �� ������� ������� �������
				{
					childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool, resultsObj, true)); // ������� ����� ����� ������ � ��������� ����� � ���������
				}

				dirsToFindInThread.clear(); // ������� ������ � ������� �� ������� ������� �������

				findFileThread(dir->dirs[i], fileName, threadPool, resultsObj, false); // ���������� ����� � ��� �� ������ 
			}
		}

		for (DirObject* nextDir : dirsToFindInThread) // ���������� ������ ����� �� ������� ������� ������� 
		{
			childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool, resultsObj, true));// ������� ����� ����� ������ � ��������� ����� � ���������
		}

		dirsToFindInThread.clear(); // ������� ������ � ������� �� ������� ������� �������

		findFileThread(dir->dirs[dir->dirs.size() - 1], fileName, threadPool, resultsObj, false); // ���������� ����� ��� ��������� ����� � ��� �� ������ 

		for (int i = 0; i < childThreads.size(); i++) // ����������� ������ ������� �������� � ������� �����
		{
			childThreads[i].join(); // ������� ��������� ������� 
		}

		childThreads.clear();
	}

	if (isNewThread) // ���� ��� ����� �����
	{
		threadPool->tryReturnThread(); // ���������� ����� ��� ������� �����
	}
	return;
}