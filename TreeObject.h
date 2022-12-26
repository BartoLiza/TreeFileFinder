#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "ThreadPool.h"

using namespace std;
namespace fs = std::filesystem; // filesystem - файловая система компьютера

class TreeObject
{
public:
	fs::path fullPath;
	string name;

	TreeObject(string fullPath);
	TreeObject(fs::path fullPath);
	TreeObject(fs::path fullPath, string name);
};

class FileObject : public TreeObject
{
public:
	FileObject(string fullPath);
	FileObject(fs::path fullPath);
};

class DirObject : public TreeObject
{
public:
	vector<DirObject*> dirs;
	vector<FileObject*> files;

	DirObject(string fullPath);
	DirObject(fs::path fullPath);

	void collectChilds();
	void collectChildsRecursive();
	bool checkDir();
	vector<string>* findFile(const string fileName, ThreadPool* threadPool);
};



void findFileThread(DirObject* dir, const string fileName, ThreadPool* threadPool);
