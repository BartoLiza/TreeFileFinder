#include "TreeObject.h"

TreeObject::TreeObject(string fullPath) : TreeObject((fs::path)fullPath)
{
}

TreeObject::TreeObject(fs::path fullPath) : TreeObject(fullPath, fullPath.stem().string())
{
}

TreeObject::TreeObject(fs::path fullPath, string name)
{
	this->fullPath = fullPath;
	this->name = name;
}


DirObject::DirObject(string fullPath) : TreeObject(fullPath) {}
DirObject::DirObject(fs::path fullPath) : TreeObject(fullPath) {}

bool DirObject::checkDir()
{
	try {
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath };
		return true;
	}
	catch (exception)
	{
		return false;
	}
}

void DirObject::collectChilds()
{
	try {
		fs::directory_iterator dirIter = fs::directory_iterator{ fullPath };

		for (fs::directory_iterator iter = fs::begin(dirIter), endIter = fs::end(dirIter); iter != endIter; iter++)
		{
			try {
				fs::directory_entry dir_entry = *iter;
				if (dir_entry.is_directory())
				{
					//DirObject* dir = new DirObject(p);
					DirObject* dir = new DirObject(dir_entry.path());
					if (dir->checkDir())
					{
						dirs.push_back(dir);
					}
				}
				else
				{
					FileObject* file = new FileObject(dir_entry.path());
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

void DirObject::collectChildsRecursive()
{
	collectChilds();
	for (DirObject* dir : dirs)
	{
		dir->collectChildsRecursive();
	}
}