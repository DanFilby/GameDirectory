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

	/// <summary>
	/// Add entry to the database, checks for validity but doesn't save to file
	/// </summary>
	void AddEntry(Entry& entry);

	EntryInfo_Short GetEntrySum(ENTRYID _id);
	EntryInfo_Short GetEntrySum(ENTRYID _id, bool& found);
	ENTRYID GetEntryId(EntryType _type, string _name, uint16_t _year);


	/// <summary>
	/// returns a count of each type of entry currently stored
	/// </summary>
	std::map<EntryType, int> GetEntryTypeCount();
	/// <summary>
	/// returns count of current entries stored
	/// </summary>
	int GetEntryCount();

	void PrintActiveEntries();

	/// <summary>
	/// Check entry is valid to add to the database
	/// </summary>
	bool IsValidEntry(const EntryInfo_Short entrySum);
	/// <summary>
	/// checks whether a matching entry is already stored
	/// </summary>
	/// <returns> true if found another matching entry </returns>
	bool IsDuplicateEntry(const EntryInfo_Short entrySum);

	/// <summary>
	/// sets a new unique id for the given entry, if valid
	/// </summary>
	/// <returns> true is successful </returns>
	bool SetUniqueId(Entry& entry);
	/// <summary>
	/// gets a unique id for the given entry, returned through out param
	/// </summary>
	/// <returns> true if successful </returns>
	bool GetUniqueId(EntryInfo_Short entrySum, int& outId);

	/// <summary>
	/// removes duplicate entries in mActiveEntries
	/// </summary>
	void RemoveDuplicates();

	/// <summary>
	/// writes all the current entries' summary to the entries file
	/// </summary>
	void UpdateEntriesFile();

	/// <summary>
	/// Checks for the correct folders, creates them if they don't exsist
	/// </summary>
	void DirectoriesCheck();

private:

	/// <summary>
	/// Reads entries file to find all saved entry's summary : id, name and type
	/// </summary>
	void LoadEntries();

	/// <summary>
	/// checks the temp ids for a match, returns true if a matching temp id exsists
	/// </summary>
	bool TempIdCheck(ENTRYID id);
	void RemoveTempId(ENTRYID id);

	//all entries found on start-up or added during the session 
	vector<EntryInfo_Short> mActiveEntries;

	vector<ENTRYID> tempIds;

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

//TODO: Add remove entry interface, by id