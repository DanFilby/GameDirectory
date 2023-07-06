#include "EntryDatabase.h"


EntryDatabase::EntryDatabase()
{
	//setup files, store in folder ids and database

	//checks neccessary folders are valid, otherwise creates them
	DirectoriesCheck();

	//reads entries file, loads all saved entry summaries into memory
	LoadEntries();
		
}

EntryDatabase::~EntryDatabase()
{
}

void EntryDatabase::AddEntry(Entry& entry)
{
	EntryInfo_Short entrySum = entry.GetSummary();

	//if not valid fail
	if (!IsValidEntry(entry.GetSummary())) { std::cout << "Failed to add entry to database\n"; return; }

	RemoveTempId(entrySum.id);

	mActiveEntries.push_back(entrySum);
	UpdateEntriesFile();
}

EntryInfo_Short EntryDatabase::GetEntrySum(ENTRYID _id)
{
	bool discard;
	return GetEntrySum(_id, discard);
}

EntryInfo_Short EntryDatabase::GetEntrySum(ENTRYID _id, bool& found)
{
	for (const auto& entry : mActiveEntries) {
		if (entry.id == _id) {
			found = true;
			return entry;
		}
	}
	found = false;
	return NULL;
}

ENTRYID EntryDatabase::GetEntryId(EntryType _type, string _name, uint16_t _year)
{
	for (const auto& entry : mActiveEntries) {
		if (entry.type == _type && entry.name == _name && entry.year == _year) {
			return entry.id;
		}
	}
	return 0;
}

int EntryDatabase::GetEntryCount()
{
	return mActiveEntries.size();
}

std::map<EntryType, int> EntryDatabase::GetEntryTypeCount()
{
	std::map<EntryType, int> entryTypeMap{ {ET_Base, 0},{ET_Game, 0}, {ET_Studio, 0 } };
	
	for (auto& entry : mActiveEntries) {
		entryTypeMap[entry.type]++;
	}

	return entryTypeMap;
}

void EntryDatabase::PrintActiveEntries()
{
	for (const auto& entry : mActiveEntries)
	{
		std::cout << "info name: " << entry.name << "\n";
		std::cout << "info id: " << entry.id << "\n";
		std::cout << "info type: " << entry.type << "\n";
		std::cout << "info year: " << entry.year << "\n\n";
	}
}

bool EntryDatabase::IsValidEntry(const EntryInfo_Short entrySum)
{
	return (!IsDuplicateEntry(entrySum) && entrySum.id != 0);
}

bool EntryDatabase::IsDuplicateEntry(const EntryInfo_Short entrySum)
{
	for (const auto& curEntry : mActiveEntries) {
		//entry is a duplicate if another entry with the same name and year exsits, checks using custom == operator
		if (entrySum == curEntry) {
			return true;
		}
	}
	return false;
}

bool EntryDatabase::SetUniqueId(Entry& entry)
{
	int uniqueId{};
	bool success = GetUniqueId(entry.GetSummary(), uniqueId);

	if (success) {
		//set the new id
		entry.mId = uniqueId;
		return true;
	}
	else { return false; }
}

bool EntryDatabase::GetUniqueId(EntryInfo_Short entrySum, int& outId)
{
	if (IsDuplicateEntry(entrySum)) { std::cout << "Entry is duplicate\n"; return false; }

	ENTRYID entryNewId = rand();
	bool idExsits;

	for (size_t i = 0; i < 50; i++)
	{
		GetEntrySum(entryNewId, idExsits);

		if (entryNewId != 0 || !idExsits || !TempIdCheck(entryNewId)) {
			break;
		}
		entryNewId = rand();
	}

	//successfully found a unique id
	if (entryNewId != 0 || !idExsits) {
		//set the new id
		outId = entryNewId;

		//store the id temporary while, so the id doesn't get given while this one is out and not stored  
		tempIds.push_back(entryNewId);

		return true;
	}
	else { return false; }
}

void EntryDatabase::DirectoriesCheck()
{
	//check data directory is valid	
	assert(IsDirVaild(DIR_PATH));

	//checks the entrees data dir 
	SetupDir(ENTRIESDATA_DIR_PATH);

}

void EntryDatabase::LoadEntries()
{
	FileReadCheck(DIR_PATH + ENTRIESLIST_FNAME);

	fstream entriesFile(DIR_PATH + ENTRIESLIST_FNAME, std::ios::in | std::ios::binary);

	//read the header into buffer
	uint16_t* fileHeaderBuf = new uint16_t[8];
	entriesFile.read((char*)fileHeaderBuf, ENTRIESHEADER_BYTESIZE * 2);

	//get info from header. (total entries, num of each type)
	EntryFileHeader entriesHeader = EntryFileHeader(fileHeaderBuf);

	//read each entry from the file into this buffer
	char* entryBuffer = new char[32];

	//loop over file reading each entree and add to active entries
	for (size_t i = 0; i < entriesHeader.totalEntries; i++)
	{
		entriesFile.read((char*)entryBuffer, EntryInfo_Short::BYTESIZE);

		EntryInfo_Short entrySummary = EntryInfo_Short(entryBuffer);

		mActiveEntries.push_back(entrySummary);
	}
}

void EntryDatabase::RemoveDuplicates()
{
	vector<EntryInfo_Short> filteredEntries{};
	bool duplicate;

	for (const auto & curEntry : mActiveEntries)
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

bool EntryDatabase::TempIdCheck(ENTRYID id)
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

void EntryDatabase::UpdateEntriesFile()
{
	ofstream entriesFile = std::ofstream(DIR_PATH + ENTRIESLIST_FNAME, std::ios::out | std::ios::binary);

	//generate the file's header, containing number of each entry stored
	EntryFileHeader entriesHeader = EntryFileHeader(GetEntryTypeCount());
	
	//write a header to the entry file to hold the counts of each entry stored
	entriesFile.write((char*)entriesHeader.ToBinary().get(), ENTRIESHEADER_BYTESIZE);

	//write twice to total 16 bytes and for validation
	entriesFile.write((char*)entriesHeader.ToBinary().get(), ENTRIESHEADER_BYTESIZE);

	//write each entry to the file. each are 32 bytes
	for (const auto& entry : mActiveEntries) {
		entriesFile.write((char*)entry.ToBinary().get(), EntryInfo_Short::BYTESIZE);
	}

}
