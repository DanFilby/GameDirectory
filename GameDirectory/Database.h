#pragma once

#pragma region FileLibs

#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#include <filesystem>
using std::filesystem::path;

#pragma endregion

#include <Windows.h>
#include <iostream>

#include "Common.h"

static const string DIR_PATH = "Data/";

class Database
{
public:

	void Init();
	void Close();

	bool IsDirVaild(string dirPath);
	void SetupDir(string dirPath);

	bool FileReadCheck(string filePath);

};

