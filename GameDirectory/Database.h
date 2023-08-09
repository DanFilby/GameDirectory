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
#include<cassert>

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
	virtual void FileDirectoriesCheck();
};


/// <summary>
/// File manager for simple string files, stores strings to a given line by line and returs using vectors
/// </summary>
class StringFileMan {
protected:

	vector<string> ReadStringFile(string filePath);
	void WriteStringFile(string filePath, vector<string> contents);

	void SortAndUniqueStrings(vector<string>& strings);

	/// <summary>
	/// reads a index string map from file, format of comma sperated pairs on new lines
	/// </summary>
	map<uint8_t, string> ReadStringIndexFile(string filePath);
	/// <summary>
	/// Writes an index and string map to file, as comma seperated pair on seperate lines
	/// </summary>
	void WriteStringIndexFile(string filePath, map<uint8_t, string> contents);

	inline uint8_t FindNewUniqueKey(string word, const map<uint8_t,string>& currentMap);

	inline uint8_t GetKey(const map<uint8_t, string>& currentMap, string value);

	inline vector<string> GetValues(const map<uint8_t, string>& currentMap);

public:
	virtual uint8_t GetKey(string value);

};


class GenreListDataBase : Database, StringFileMan {

	const string GENRELIST_FNAME = "Genre-List.dat";

public:
	static const uint8_t MAXCOUNT_GENRE = 255;
	static const uint8_t MAXLEN_GENRENAME = 32;

public:
	GenreListDataBase();

	void FileDirectoriesCheck();

	string GetGenre(uint8_t key);
	uint8_t GetKey(string genre);

	bool GenreExsists(uint8_t genreKey);
	bool GenreExsists(string genre);

	void AddGenre(string genre);
	void RemoveGenre(string genre);

	vector<string> GetAllGenres();
	void PrintGenreList();


	void LoadGenres();
	void UpdateGenreListFile();

private:
	map<uint8_t, string> mGenreList;

};

class TagListDataBase : Database, StringFileMan {

	const string TAGLIST_FNAME = "Tag-List.dat";

public:
	static const uint8_t MAXCOUNT_TAGS = 255;
	static const uint8_t MAXLEN_TAGS = 20;

public:
	TagListDataBase();

	void FileDirectoriesCheck();

	string GetTag(uint8_t key);
	uint8_t GetKey(string tag);

	bool TagExsists(uint8_t tagKey);
	bool TagExsists(string tag);

	void AddTag(string tag);
	void RemoveTag(string tag);

	vector<string> GetAllTags();
	void PrintTagList();


	void LoadTags();
	void UpdateTagListFile();

private:
	map<uint8_t, string> mTagList;
};