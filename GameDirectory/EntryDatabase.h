#pragma once	

#pragma region stdlib
#include <vector>
using std::vector;

#include <istream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#pragma endregion

#include "Entries.h"

class EntryDatabase
{
	//entries.dat contains each entry's name and link to its folder location. dupes?

	const string DIR_PATH = "/Data/";
	const string ENTRIES_DIR_PATH = DIR_PATH + "Entries/";
	const string ENTRIESLIST_FNAME = "Entries.dat";

public: 
	EntryDatabase();
	~EntryDatabase();

private:
	//all entries found on start-up or added during the session 
	vector<ENTRYID> mActiveEntries;


};

