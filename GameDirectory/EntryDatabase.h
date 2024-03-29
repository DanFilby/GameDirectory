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
//TODO: update this comment
//TODO: Add, remove, and edit entry interface, by id

struct EntryDataPath {
	static const uint8_t BYTESIZE = 4;

	uint16_t ParentDirIndex; //0-65536
	uint8_t EntryDirIndex;	//0-255
	bool EntryHasData;

	EntryDataPath()
		:EntryHasData(false), ParentDirIndex(0), EntryDirIndex(0) {
	}

	EntryDataPath(bool _hasData, uint16_t _parentIndex, uint8_t _dirIndex) 
		:EntryHasData(_hasData), ParentDirIndex(_parentIndex), EntryDirIndex(_dirIndex){
	}

	EntryDataPath(char* binDat) {
		ParentDirIndex = binDat[0];
		EntryDirIndex = binDat[2];
		EntryHasData = binDat[3];
	}

	shared_ptr<char[]> ToBinary() {
		char* binData = new char[4];
		std::memcpy(&binData[0], &ParentDirIndex, sizeof(uint16_t));
		binData[2] = (char)EntryDirIndex;
		binData[3] = (char)EntryHasData;
		return shared_ptr<char[]>(binData);
	}
};

//manages all entry relation files
class FileManager_EntryRelations {

public:
	FileManager_EntryRelations(const map<ENTRYID, EntryDataPath>& _dataPathsMap, string(*getEntryDataPath)(const EntryDataPath dataPath, const EntryType entryType));

	void Write_EntryRelationFile(EntryInfo_Short entrySum, EntryRelations entryRelations);
	EntryRelations Read_EntryRelationFile(EntryInfo_Short entrySum, EntryRelationsType relationType);

private:

	const map<ENTRYID, EntryDataPath>& mDataPathsMap;
	string(*mGetEntryDataPath)(const EntryDataPath dataPath, EntryType entryType);

	inline void EnsureRelationsSizeValid(vector<ENTRYID>& relations, EntryRelationsType relationsType);

	inline string RelationsFilePath(string entryDirPath, ENTRYID entryId, EntryRelationsType type);
	void GetEntryRelationsFile(fstream& entryRelationsFile, EntryInfo_Short entrySum, EntryRelationsType relationsType);

	inline void WriteFileHeader(fstream& file, const uint16_t& relationCount, const EntryRelationsType& type);
	inline void ReadFileHeader(fstream& file, uint16_t& relationCount, EntryRelationsType& type);

	void WriteFileBody(fstream& file, const vector<ENTRYID>& relations);
};

template<class EntryTy>
struct Entry_Editor {
	shared_ptr<EntryTy> entry;
	EntryDatabase* EDatabase;
	typedef void (EntryDatabase::* WriteEntryDataFunction)(shared_ptr<Entry>);
	WriteEntryDataFunction WriteEntryDataFuncPtr;

	Entry_Editor():entry(nullptr), EDatabase(nullptr), WriteEntryDataFuncPtr(nullptr){}

	Entry_Editor(shared_ptr<EntryTy> _entry, EntryDatabase* _database, WriteEntryDataFunction _updateEntryFuncPtr)
		:entry(_entry), EDatabase(_database), WriteEntryDataFuncPtr(_updateEntryFuncPtr)
	{}
	
	void UpdateDatabase() {
		if (EDatabase) { (EDatabase->*WriteEntryDataFuncPtr)(dynamic_pointer_cast<Entry>(entry)); }
	}
};

class EntryDatabase : Database
{
	//entries.dat contains each entry's name and link to its folder location. dupes?
	const string ENTRIESLIST_FNAME = "Entries.dat";

	inline static const string ENTRIESDATA_DIR_PATH = APP_DATA_DIR_PATH + "Entries-Data/";
	inline static const string ENTRIESDATA_GAME_DIRNAME = "Game-Entries/";
	inline static const string ENTRIESDATA_STUDIO_DIRNAME = "Studio-Entries/";

	const uint8_t ENTRYSTORAGESIZE = EntryDataPath::BYTESIZE + EntryInfo_Short::BYTESIZE;

public: 
	EntryDatabase(shared_ptr<GenreListDataBase> _genreDatabase, shared_ptr<TagListDataBase> _tagDatabase);
	~EntryDatabase();

	void FileDirectoriesCheck();
	void LoadEntriesFile();
	void UpdateEntriesFile();

	template<typename EntryTy> bool ReadEntryData(const EntryInfo_Short& entrySum, shared_ptr<char[]>& outBinData);
	void WriteEntryData(shared_ptr<Entry> _Entry, const EntryDataPath& _dataPath);

	template<typename EntryTy> void ReadRelationsIntoEntry(shared_ptr<EntryTy> _Entry); 
	void WriteRelations(shared_ptr<Entry> _Entry);

	void AddEntry(shared_ptr<Entry> _entry);
	template<class EntryTy> void AddEntry(shared_ptr<EntryTy> _entry) { AddEntry(dynamic_pointer_cast<Entry>(_entry)); }

	void UpdateEntryData(shared_ptr<Entry> _entry);
	bool GetEntryAndData(ENTRYID _entryId, shared_ptr<Entry>& outEntry);

	template<class EntryTy>
	Entry_Editor<EntryTy> EditEntry(ENTRYID _entryId) {
		shared_ptr<Entry> entry;
		if (GetEntryAndData(_entryId, entry)) {
			return Entry_Editor(dynamic_pointer_cast<EntryTy>(entry), this, &EntryDatabase::UpdateEntryData);
		}
		else { return Entry_Editor<EntryTy>(); }
	}

	void RemoveEntry(ENTRYID _entryId);
	template<typename EntryTy> void RemoveEntry(shared_ptr<EntryTy> _entry) { RemoveEntry(dynamic_pointer_cast<Entry>(_entry)->Id()); }

	bool EntryExsists(ENTRYID _id);
	bool IsValidEntry(Entry* _entry);
	bool IsDuplicateEntry(Entry* _entry);
	void RemoveDuplicates();

	bool GetUniqueId(Entry& _entry, int& outId);
	bool SetUniqueId(Entry& entry);

	inline bool DataPathExsists(ENTRYID _entryId);
	EntryDataPath GenerateDataPath(ENTRYID _entryId);
	EntryDataPath GetDataPath(ENTRYID _entryId);

	static string GetParentDirNameFromType(const EntryType entryType);
	static string GetEntryData_ParentDirPath(const EntryDataPath dataPath, const EntryType entryType);

	static string GetEntryData_DirPath(const EntryDataPath dataPath, const EntryType entryType);
	string GetEntryData_DirPath(const ENTRYID entryId);

	string GetEntryData_FilePath(EntryDataPath dataPath, ENTRYID _entryId, string _entryName);
	string GetEntryData_FilePath(EntryInfo_Short entrySum);

	shared_ptr<GameEntry> GetGameEntry(ENTRYID _id);
	shared_ptr<StudioEntry> GetStudioEntry(ENTRYID _id);

	Entry GetBaseEntry(ENTRYID _id);
	EntryInfo_Short GetEntrySummary(ENTRYID _id);
	ENTRYID GetEntryId(EntryType _type, string _name, uint16_t _year);

	EntryType GetEntryType(ENTRYID _id);
	std::map<EntryType, uint16_t> GetEntryTypeCount();
	int GetEntryCount();

	void PrintActiveEntries();
	void PrintEntriesPaths();

private:
	int GetEntryListIndex(ENTRYID _id);
	bool IsDuplicateTempId(ENTRYID id);
	void RemoveTempId(ENTRYID id);

	//all entries found on start-up or added during the session 
	vector<Entry> mActiveEntries;

	map<ENTRYID, EntryDataPath> mEntryDataPaths;

	vector<ENTRYID> tempIds;

	shared_ptr<GenreListDataBase> mGenreDatabase;
	shared_ptr<TagListDataBase> mTagDatabase;

	FileManager_EntryRelations mEntryRelationsFileManager;
};

//header for the entries file, contains count of each type of entry
struct EntryFileHeader {
	static const uint8_t BYTESIZE = 8;

	EntryFileHeader(std::map<EntryType, uint16_t> entries) {
		baseEntries = entries[ET_Base];
		gameEntries = entries[ET_Game];
		studioEntries = entries[ET_Studio];
		totalEntries = baseEntries + gameEntries + studioEntries;
	}

	EntryFileHeader(char* binHeader) {
		totalEntries = binHeader[0]; baseEntries = binHeader[2];
		gameEntries = binHeader[4]; studioEntries = binHeader[6];
	}

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
		shared_ptr<char[]> binary = shared_ptr<char[]>((char*)&binDat[0]);

		return binary;
	}

	uint16_t totalEntries, baseEntries, gameEntries, studioEntries;
};