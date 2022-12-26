#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

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
};
