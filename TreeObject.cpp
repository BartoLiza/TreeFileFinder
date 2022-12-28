#include "TreeObject.h"
#include "ThreadPool.h"

TreeObject::TreeObject(string fullPath) : TreeObject((fs::path)fullPath)
{
}

TreeObject::TreeObject(fs::path fullPath) : TreeObject(fullPath, fullPath.filename().string()) // filename() - возвращает имя с расширением
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

DirObject::~DirObject()
{
	for (DirObject* dir : dirs)
	{
		delete(dir);
	}
	dirs.clear();

	for (FileObject* file : files)
	{
		delete(file);
	}
	files.clear();
}

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

void DirObject::findFile(const string fileName, ThreadPool* threadPool, ResultsObject* resultsObj)
{
	findFileThread(this, fileName, threadPool, resultsObj, false); // начинаем поиск в главном потоке
}

void findFileThread(DirObject* dir, const string fileName, ThreadPool* threadPool, ResultsObject* resultsObj, bool isNewThread) // функция потока поиска
{
	for (FileObject* file : dir->files) // перебираем файлы в текущей папке
	{
		if (file->name == fileName) // если найдено совпадение имени файла
		{
			resultsObj->putResult(file->fullPath.string()); //кладем путь найденого файла в результаты
		}
	}

	if (dir->dirs.size() > 0) // если есть вложенные папки
	{
		vector<DirObject*> dirsToFindInThread; // вектор с папками на которые хватило потоков

		vector<thread> childThreads;// вектор с потоками в данном узле 


		for (int i = 0; i + 1 < dir->dirs.size(); i++)
		{
			if (threadPool->tryGetThread())
			{
				dirsToFindInThread.push_back(dir->dirs[i]); // добавляем папку в вектор на которую хватило потока
			}
			else
			{

				for (DirObject* nextDir : dirsToFindInThread) // перебираем вектор папок на которые хватило потоков
				{
					childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool, resultsObj, true)); // создаем новый поток поиска в следующей папке и добавляем
				}

				dirsToFindInThread.clear(); // очищаем вектор с папками на которые хватило потоков

				findFileThread(dir->dirs[i], fileName, threadPool, resultsObj, false); // продолжаем поиск в том же потоке 
			}
		}

		for (DirObject* nextDir : dirsToFindInThread) // перебираем вектор папок на которые хватило потоков 
		{
			childThreads.push_back(thread(findFileThread, nextDir, fileName, threadPool, resultsObj, true));// создаем новый поток поиска в следующей папке и добавляем
		}

		dirsToFindInThread.clear(); // очищаем вектор с папками на которые хватило потоков

		findFileThread(dir->dirs[dir->dirs.size() - 1], fileName, threadPool, resultsObj, false); // продолжаем поиск для последней папки в том же потоке 

		for (int i = 0; i < childThreads.size(); i++) // перебираеем вектор потоков созданых к текущей папке
		{
			childThreads[i].join(); // ожидаем окончание потоков 
		}

		childThreads.clear();
	}

	if (isNewThread) // если это новый поток
	{
		threadPool->tryReturnThread(); // возвращаем поток для текущей папки
	}
	return;
}