#pragma once	

#pragma region filestreams

#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#pragma endregion

#include <Windows.h>
#include <iostream>
#include <map>

#include "Common.h"
#include "Entries.h"

/*
* Entries.dat - holds the entries's ids, name, and their type (game, studio etc)
*   ->starts with a header giving amount of entries to read
* 
* Entries-Data/ contains files holding entries data.
*	-> each entries-data file will start with a header containing amount of entries
*/

struct EntryFileHeader;

class EntryDatabase
{
	//entries.dat contains each entry's name and link to its folder location. dupes?

	const string DIR_PATH = "Data/";
	const string ENTRIESDATA_DIR_PATH = DIR_PATH + "Entries-Data/";
	const string ENTRIESLIST_FNAME = "Entries.dat";

	const uint8_t ENTRIESHEADER_BYTESIZE = 4 * sizeof(uint16_t);

public: 
	EntryDatabase();
	~EntryDatabase();

	int GetEntryCount();
	std::map<EntryType, int> GetEntryTypeCount();

	/// <summary>
	/// writes all the current entries' summary to the entries file
	/// </summary>
	void UpdateEntriesFile();

private:
	
	/// <summary>
	/// Checks for the correct folders, creates them if they don't exsist
	/// </summary>
	void FolderSetup();
	/// <summary>
	/// Checks a folder exsists, creates one if not
	/// </summary>
	void CheckFolder(string folderPath);

	/// <summary>
	/// Reads entries file to find all saved entry's summary : id, name and type
	/// </summary>
	void LoadEntries();


	//all entries found on start-up or added during the session 
public:	vector<EntryInfo_Short> mActiveEntries;

};


//header for the entries file, contains count of each type of entry
struct EntryFileHeader {

	//load the header from current entries dict
	EntryFileHeader(std::map<EntryType, int> entries) {
		baseEntries = (uint16_t)entries[ET_Base]; 
		gameEntries = (uint16_t)entries[ET_Game];
		studioEntries = (uint16_t)entries[ET_Studio];
		totalEntries = baseEntries + gameEntries + studioEntries;
	}

	//load the header back from binary format
	EntryFileHeader(uint16_t* binHeader) {
		totalEntries = binHeader[0]; baseEntries = binHeader[1];
		gameEntries = binHeader[2]; studioEntries = binHeader[3];
	}

	/// <summary>
	/// convert the header into binary: (8 bytes - four 2 byte uints)
	/// </summary>
	unique_ptr<uint16_t[]> ToBinary() {

		//init an 8 byte header: **| num of entries | num of base entries | num of game entries | num of studio entries |**
		unique_ptr<uint16_t[]> bin = unique_ptr<uint16_t[]>(
			new uint16_t[4]{ totalEntries, baseEntries, gameEntries, studioEntries });

		return bin;
	}

	uint16_t totalEntries, baseEntries, gameEntries, studioEntries;
};

