#pragma once

#pragma region filestreams

#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#pragma endregion

#include <Windows.h>
#include <iostream>
#include <filesystem>

#include "Common.h"

static const string DIR_PATH = "Data/";

class Database
{
public:

	void Init();
	void Close();

	bool IsDirVaild(string dirPath);
	void SetupDir(string dirPath);

};

