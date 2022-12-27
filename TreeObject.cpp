#include "TreeObject.h"
#include "ThreadPool.h"
#include "ResultsObject.h"

TreeObject::TreeObject(string fullPath) : TreeObject((fs::path)fullPath)
{
}

TreeObject::TreeObject(fs::path fullPath) : TreeObject(fullPath, fullPath.stem().string()) // stem() - возвращает только имя
{
}

TreeObject::TreeObject(fs::path fullPath, string name)
{
	this->fullPath = fullPath;
	this->name = name;
}

FileObject::FileObject(string fullPath) : TreeObject(fullPath) {} //вывызается родительский конструктор
FileObject::FileObject(fs::path fullPath) : TreeObject(fullPath) {}

DirObject::DirObject(string fullPath) : TreeObject(fullPath) {}
DirObject::DirObject(fs::path fullPath) : TreeObject(fullPath) {}

bool DirObject::checkDir()
{
	try {
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath }; // проверяем доступность содержимого папки
		return true; // доступно!
	}
	catch (exception)
	{
		return false; // не доступно! (системные файлы, которые мы не можем открывать)
	}
}

void DirObject::collectChilds()
{
	try { // проверка папки на доступность открытия
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath }; //  directory_iterator - указатель на пути внутри папки

		for (fs::directory_iterator iter = fs::begin(dirIter), endIter = fs::end(dirIter); iter != endIter; iter++) // сбор указателей на содержимое папки
		{
			try {
				fs::directory_entry dir_entry = *iter; // directory_entry - папка или файл внутри папки, берем текущий элемент
				if (dir_entry.is_directory()) // является ли текущий элемент папкой?
				{
					DirObject* dir = new DirObject(dir_entry.path()); // создаем новый элемент дерева (класса папки) 
					if (dir->checkDir()) // если доступна
					{
						dirs.push_back(dir); // кладем папку в дерево
					}
				}
				else
				{
					FileObject* file = new FileObject(dir_entry.path());// создаем новый элемент дерева (класса файла) 
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
	for (DirObject* dir : dirs) // перебираем все элементы ввектора dirs
	{
		dir->collectChildsRecursive();
	}
}

vector<string>* DirObject::findFile(const string fileName, ThreadPool* threadPool)
{
	if (threadPool->tryGetThread()) // если есть сободный поток
	{
		thread thr(findFileThread, this, fileName, threadPool); // создаем новый поток и передаем в функцию потока текущий узел, имя искомого файла и пул потоков
		thr.join(); // ожидаем окончание выполнения потока
	}

	vector<string>* res = results.getResults(); // 
	return res;
}

void findFileThread(DirObject* dir, const string fileName, ThreadPool* threadPool) // функция потока поиска
{
	for (FileObject* file : dir->files) // перебираем файлы в текущей папке
	{
		if (file->name == fileName) // если найдено совпадение имени файла
		{
			results.putResult(file->fullPath.string()); //кладем путь найденого файла в результаты
		}
	}

	if (dir->dirs.size() > 0) // если кол-во папок 
	{
		vector<DirObject*> dirsToFindInThread;

		vector<thread> childThreads;


		for (int i = 0; i + 1 < dir->dirs.size(); i++)
		{
			if (threadPool->tryGetThread())
			{
				dirsToFindInThread.push_back(dir->dirs[i]);
			}
			else
			{

				for (DirObject* nextDir : dirsToFindInThread)
				{
					childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool));
				}

				dirsToFindInThread.clear();

				findFileThread(dir->dirs[i], fileName, threadPool);
			}
		}

		for (DirObject* nextDir : dirsToFindInThread)
		{
			childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool));
		}

		dirsToFindInThread.clear();

		findFileThread(dir->dirs[dir->dirs.size() - 1], fileName, threadPool);

		for (int i = 0; i < childThreads.size(); i++)
		{
			childThreads[i].join();
		}
	}

	threadPool->tryReturnThread();
	return;
}