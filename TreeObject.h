#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "ThreadPool.h"

using namespace std;
namespace fs = std::filesystem; // filesystem - файловая система компьютера

class TreeObject // базовый класс узлов дерева
{
public:
	fs::path fullPath;
	string name;

	TreeObject(string fullPath);
	TreeObject(fs::path fullPath);
	TreeObject(fs::path fullPath, string name);
};

class FileObject : public TreeObject // узел, который обозначает файл
{
public:
	FileObject(string fullPath);
	FileObject(fs::path fullPath);
};

class DirObject : public TreeObject
{
public:
	vector<DirObject*> dirs; // вложенные папки
	vector<FileObject*> files;// вложенные файлы

	DirObject(string fullPath);
	DirObject(fs::path fullPath);

	void collectChilds(); // собираем дочерние узлы из файловой системы
	void collectChildsRecursive(); // собираем дочерние узлы из файловой системы ( в случае, если в папке содержится папка)
	bool checkDir(); // провека пути папки на доступность в системе
	vector<string>* findFile(const string fileName, ThreadPool* threadPool);
};



void findFileThread(DirObject* dir, const string fileName, ThreadPool* threadPool);
