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

//path to data directory
static const string DIR_PATH = "Data/";

/// <summary>
/// Database base class provides usefull functions for any class needing to store data
/// </summary>
class Database
{
public:
	/// <summary>
	/// Checks a directory exsits at the given path
	/// </summary>
	bool IsDirVaild(string dirPath);
	/// <summary>
	/// ensures a directory exsits at the given path, by creating one if needed
	/// </summary>
	void SetupDir(string dirPath);
	/// <summary>
	/// Checks the file is valid, and not empty
	/// </summary>
	bool FileReadCheck(string filePath);

	/// <summary>
	/// Checks directories are valid
	/// </summary>
	virtual void DirectoriesCheck();
};

/// <summary>
/// Database manager for the whole application, checks any neccessary directories on start-up
/// </summary>
class DatabaseManager : Database {
public:
	/// <summary>
	/// Inits databases for the entire application, validation checks
	/// </summary>
	void AppInit();

	void Close();
};
//TODO: Might need to move this to new header and contain the other databases to setup genre and tag


/// <summary>
/// File manager for simple string files, stores strings to a given line by line and returs using vectors
/// </summary>
class StringFileMan {
protected:
	/// <summary>
	/// Loads a string file into vector
	/// </summary>
	vector<string> ReadStringFile(string filePath);

	/// <summary>
	/// Writes a string vector to file, strings are inputed line by line
	/// </summary>
	void WriteStringFile(string filePath, vector<string> contents);

	/// <summary>
	/// Sorts strings and removes duplicates
	/// </summary>
	void SortAndUniqueStrings(vector<string>& strings);

	/// <summary>
	/// reads a index string map from file, format of comma sperated pairs on new lines
	/// </summary>
	map<uint8_t, string> ReadStringIndexFile(string filePath);

	/// <summary>
	/// Writes an index and string map to file, as comma seperated pair on seperate lines
	/// </summary>
	void WriteStringIndexFile(string filePath, map<uint8_t, string> contents);

	uint8_t FindNewUniqueKey(string word, const map<uint8_t,string>& currentMap);
};

