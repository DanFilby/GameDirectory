#pragma once	

#include<cassert>
#include <map>

#include "Common.h"
#include "Entries.h"
#include "Database.h"

/*
* Entries.dat - holds the entries's ids, name, and their type (game, studio etc)
*   -> starts with a header giving amount of entries to read
*   -> each entry's summary is saved one after the other in binary format of 32 bytes
* 
* Entries-Data/ folder containing each entry's full data in their own file named with their unique id 
*	-> each entry data file format will consist of blocks of data i.e rating, pictures
* 
* Entries-Data/Pictures/ folder containg all pictures
*   -> optimse this as will have to loop over all pictures to find the right one
*/

struct EntryFileHeader;

class EntryDatabase : Database
{
	//entries.dat contains each entry's name and link to its folder location. dupes?

	const string ENTRIESDATA_DIR_PATH = DIR_PATH + "Entries-Data/";
	const string ENTRIESLIST_FNAME = "Entries.dat";

	const uint8_t ENTRIESHEADER_BYTESIZE = 4 * sizeof(uint16_t);

public: 
	EntryDatabase();
	~EntryDatabase();

	void FileDirectoriesCheck();
	void LoadEntriesFile();
	void UpdateEntriesFile();

	void AddEntry(Entry entry);

	void RemoveEntry(const Entry& _entry);
	void RemoveEntry(ENTRYID _entryId);

	bool EntryExsists(ENTRYID _id);
	bool IsValidEntry(Entry& _entry);
	bool IsDuplicateEntry(Entry& _entry);
	void RemoveDuplicates();

	bool GetUniqueId(Entry& _entry, int& outId);
	bool SetUniqueId(Entry& entry);

	Entry GetEntry(ENTRYID _id);
	EntryInfo_Short GetEntrySummary(ENTRYID _id);
	ENTRYID GetEntryId(EntryType _type, string _name, uint16_t _year);

	std::map<EntryType, uint16_t> GetEntryTypeCount();
	int GetEntryCount();

	void PrintActiveEntries();

private:
	bool IsDuplicateTempId(ENTRYID id);
	void RemoveTempId(ENTRYID id);

	//all entries found on start-up or added during the session 
	vector<Entry> mActiveEntries;

	vector<ENTRYID> tempIds;
};


//header for the entries file, contains count of each type of entry
struct EntryFileHeader {

	//load the header from current entries dict
	EntryFileHeader(std::map<EntryType, uint16_t> entries) {
		baseEntries = entries[ET_Base]; 
		gameEntries = entries[ET_Game];
		studioEntries = entries[ET_Studio];
		totalEntries = baseEntries + gameEntries + studioEntries;
	}

	//load the header back from binary format
	EntryFileHeader(char* binHeader) {
		totalEntries = binHeader[0]; baseEntries = binHeader[2];
		gameEntries = binHeader[4]; studioEntries = binHeader[6];
	}

	//load the header back from uint16_t array format
	EntryFileHeader(uint16_t* header) {
		totalEntries = header[0]; baseEntries = header[1];
		gameEntries = header[2]; studioEntries = header[3];
	}

	/// <summary>
	/// convert the header into binary: (8 bytes - four 2 byte uints)
	/// </summary>
	shared_ptr<char[]> ToBinary() {
		//init an 8 byte header: **| num of entries | num of base entries | num of game entries | num of studio entries |**
		uint16_t* binDat = new uint16_t[4]{ totalEntries, baseEntries, gameEntries, studioEntries };

		//cast to byte array
		shared_ptr<char[]> binary = shared_ptr<char[]>((char*) & binDat[0]);

		return binary;
	}

	uint16_t totalEntries, baseEntries, gameEntries, studioEntries;
};

//TODO: Add, remove, and edit entry interface, by id