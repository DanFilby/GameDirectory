#include "EntryDatabase.h"


EntryDatabase::EntryDatabase()
{
	//setup files, store in folder ids and database

	//checks neccessary folders are valid, otherwise creates them
	FileDirectoriesCheck();

	//reads entries file, loads all saved entry summaries into memory
	LoadEntriesFile();
		
}

EntryDatabase::~EntryDatabase()
{
}

void EntryDatabase::FileDirectoriesCheck()
{
	//check data directory is valid	
	assert(IsDirVaild(DIR_PATH));

	SetupDir(ENTRIESDATA_DIR_PATH);
	SetupDir(ENTRIESDATA_DIR_PATH + ENTRIESDATA_GAME_DIRNAME);
}

void EntryDatabase::LoadEntriesFile()
{
	FileReadCheck(DIR_PATH + ENTRIESLIST_FNAME);

	fstream entriesFile(DIR_PATH + "TEST_" + ENTRIESLIST_FNAME, std::ios::in | std::ios::binary);

	//read the header into buffer
	uint16_t* fileHeaderBuf = new uint16_t[8];
	entriesFile.read((char*)fileHeaderBuf, EntryFileHeader::BYTESIZE * 2);

	//get info from header. (total entries, num of each type)
	EntryFileHeader entriesHeader = EntryFileHeader(fileHeaderBuf);

	//read each entry from the file into this buffer
	char* entryBuffer = new char[ENTRYSTORAGESIZE];

	//loop over file reading each entree and add to active entries
	for (size_t i = 0; i < entriesHeader.totalEntries; i++)
	{
		entriesFile.read((char*)entryBuffer, ENTRYSTORAGESIZE);

		EntryInfo_Short entrySummary = EntryInfo_Short(&entryBuffer[0]);
		EntryDataPath dataPath = EntryDataPath(&entryBuffer[EntryInfo_Short::BYTESIZE]);

		Entry entry = Entry(entrySummary);

		std::cout << "DataPath: " << dataPath.EntryHasData << "  " << dataPath.ParentDirIndex << "  " << dataPath.DirIndex << "\n";

		mActiveEntries.push_back(entry);
	}
}

void EntryDatabase::UpdateEntriesFile()
{
	ofstream entriesFile = std::ofstream(DIR_PATH + "TEST_" + ENTRIESLIST_FNAME, std::ios::out | std::ios::binary);
	if (!entriesFile.good()) { std::cout << "Failed to write to file: " << DIR_PATH + ENTRIESLIST_FNAME << "\n"; return; }

	//generate the file's header, containing number of each entry stored
	EntryFileHeader entriesHeader = EntryFileHeader(GetEntryTypeCount());

	//write a header to the entry file to hold the counts of each entry stored
	entriesFile.write((char*)entriesHeader.ToBinary().get(), EntryFileHeader::BYTESIZE);

	//write twice to total 16 bytes and for validation
	entriesFile.write((char*)entriesHeader.ToBinary().get(), EntryFileHeader::BYTESIZE);

	char* entryBuffer = new char[ENTRYSTORAGESIZE];

	//write each entry to the file. each are 36 bytes
	for (const auto& entry : mActiveEntries) {

		EntryDataPath dataPath(false,2,3);
		if (EntryDataPaths.count(entry.mId) > 0) {
			dataPath = EntryDataPaths[entry.mId];
		}

		//copy both entry summary and entry data path into 
		std::memcpy(&entryBuffer[0], &entry.GetSummary().ToBinary()[0], EntryInfo_Short::BYTESIZE);
		std::memcpy(&entryBuffer[EntryInfo_Short::BYTESIZE], &dataPath.ToBinary()[0], EntryDataPath::BYTESIZE);

		entriesFile.write(entryBuffer, ENTRYSTORAGESIZE);
	}
	delete[](entryBuffer);
}

void EntryDatabase::AddEntry(Entry entry)
{
	if (!IsValidEntry(entry)) { std::cout << "Failed to add entry to database\n"; return; }

	mActiveEntries.push_back(Entry(entry));

	RemoveTempId(entry.mId);
	UpdateEntriesFile();
}

void EntryDatabase::RemoveEntry(const Entry& _entry)
{
	RemoveEntry(_entry.mId);
	UpdateEntriesFile();
}

void EntryDatabase::RemoveEntry(ENTRYID entryId)
{
	int entryIndex = -1;
	for (size_t i = 0; i < mActiveEntries.size(); i++)
	{
		if (mActiveEntries[i].mId == entryId) {
			entryIndex = i;
			break;
		}
	}

	if (entryIndex != -1) {
		mActiveEntries.erase(mActiveEntries.begin() + entryIndex);
	}
}

bool EntryDatabase::EntryExsists(ENTRYID _id)
{
	for (const auto& entry : mActiveEntries) {
		if (entry.mId == _id) {
			return true;
		}
	}
	return NULL;
}

bool EntryDatabase::IsValidEntry(Entry& _entry)
{
	return (!IsDuplicateEntry(_entry) && _entry.mId != 0);
}

bool EntryDatabase::IsDuplicateEntry(Entry& _entry)
{
	for (auto& curEntry : mActiveEntries) {
		//entry is a duplicate if another entry with the same name and year exsits, checks using custom == operator
		if (_entry == curEntry) {
			return true;
		}
	}
	return false;
}

void EntryDatabase::RemoveDuplicates()
{
	vector<Entry> filteredEntries{};
	bool duplicate;

	for (const auto& curEntry : mActiveEntries)
	{
		duplicate = false;
		for (const auto& addedEntry : filteredEntries)
		{
			if (curEntry == addedEntry) {
				duplicate = true;
				break;
			}
		}
		if (!duplicate) {
			filteredEntries.push_back(curEntry);
		}
	}
	mActiveEntries = filteredEntries;
}

bool EntryDatabase::GetUniqueId(Entry& _entry, int& outId)
{
	if (IsDuplicateEntry(_entry)) { std::cout << "Entry is duplicate\n"; return false; }

	ENTRYID entryNewId = rand();

	for (size_t i = 0; i < 50; i++)
	{
		if (entryNewId != 0 || !EntryExsists(entryNewId) || !IsDuplicateTempId(entryNewId)) {
			break;
		}
		entryNewId = rand();
	}

	//successfully found a unique id
	if (entryNewId != 0 || !EntryExsists(entryNewId)) {
		outId = entryNewId;
		tempIds.push_back(entryNewId);

		return true;
	}
	else { return false; }
}

bool EntryDatabase::SetUniqueId(Entry& entry)
{
	int uniqueId{};
	bool success = GetUniqueId(entry, uniqueId);

	if (success) {
		//set the new id
		entry.mId = uniqueId;
		return true;
	}
	else { return false; }
}

Entry EntryDatabase::GetEntry(ENTRYID _id)
{
	for (auto& entry : mActiveEntries) {
		if (entry.mId == _id) {
			return entry;
		}
	}

	std::cout << "Unable to find entry in database\n";
	return Entry();
}

EntryInfo_Short EntryDatabase::GetEntrySummary(ENTRYID _id)
{
	return GetEntry(_id).GetSummary();
}

ENTRYID EntryDatabase::GetEntryId(EntryType _type, string _name, uint16_t _year)
{
	for (const auto& entry : mActiveEntries) {
		if (entry.mType == _type && entry.mName == _name && entry.mYear == _year) {
			return entry.mId;
		}
	}
	return 0;
}

std::map<EntryType, uint16_t> EntryDatabase::GetEntryTypeCount()
{
	std::map<EntryType, uint16_t> entryTypeMap{ {ET_Base, 0},{ET_Game, 0}, {ET_Studio, 0 } };

	for (auto& entry : mActiveEntries) {
		entryTypeMap[entry.mType]++;
	}

	return entryTypeMap;
}

int EntryDatabase::GetEntryCount()
{
	return mActiveEntries.size();
}


void EntryDatabase::PrintActiveEntries()
{
	for (const auto& entry : mActiveEntries)
	{
		std::cout << "Entry " << entry.mId << " :\n";
		std::cout << "info name: " << entry.mName << "\n";
		std::cout << "info type: " << entry.mType << "\n";
		std::cout << "info year: " << entry.mYear << "\n\n";
	}
}

bool EntryDatabase::IsDuplicateTempId(ENTRYID id)
{
	for (auto& tempId : tempIds) {
		if (tempId == id) {
			return true;
		}
	}
	return false;
}

void EntryDatabase::RemoveTempId(ENTRYID id)
{
	int index = -1, counter = 0;
	for (auto& tempId : tempIds) {
		if (tempId == id) {
			index = counter;
		}
		counter++;
	}
	//if found, remove
	if (index != -1) {
		tempIds.erase(tempIds.begin() + index);
	}

}

