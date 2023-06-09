#pragma once	

#pragma region filestreams

#include <istream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#pragma endregion

#include "Common.h"
#include "Entries.h"

/*
* Entries.dat - holds the entries's ids, name, and their type (game, studio etc)
*   ->starts with a header giving amount of entries to read
* 
* Entries-Data/ contains files holding entries data.
*	-> each entries-data file will start with a header containing amount of entries
*/


class EntryDatabase
{
	//entries.dat contains each entry's name and link to its folder location. dupes?

	const string DIR_PATH = "/Data/";
	const string ENTRIES_DIR_PATH = DIR_PATH + "Entries-Data/";
	const string ENTRIESLIST_FNAME = "Entries.dat";

public: 
	EntryDatabase();
	~EntryDatabase();

private:
	//all entries found on start-up or added during the session 
	vector<ENTRYID> mActiveEntries;


};

