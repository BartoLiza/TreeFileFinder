// TreeFileFinder.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "TreeObject.h"


#define PATH_PARAM "--path"
#define DEFAULT_BASE_PATH "C:/"

#define THREADS_COUNT_PARAM "--num_threads"
#define DEFAULT_THREADS_COUNT 10

using namespace std;
namespace fs = std::filesystem;

bool collect(DirObject* baseDirObj);
string find(string fileName, DirObject* baseDirObj, ThreadPool* threadPool);