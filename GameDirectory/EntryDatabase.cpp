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

	fstream entriesFile(DIR_PATH + ENTRIESLIST_FNAME, std::ios::in | std::ios::binary);

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
		Entry entry = Entry(entrySummary);
		mActiveEntries.push_back(entry);

		EntryDataPath entryDataPath = EntryDataPath(&entryBuffer[EntryInfo_Short::BYTESIZE]);
		mEntryDataPaths.emplace(entry.mId, entryDataPath);
		std::cout << "DataPath: " << entryDataPath.EntryHasData << "  " << entryDataPath.ParentDirIndex << "  " << (int)entryDataPath.EntryDirIndex << "\n";
	


	}
}

void EntryDatabase::UpdateEntriesFile()
{
	ofstream entriesFile = std::ofstream(DIR_PATH + ENTRIESLIST_FNAME, std::ios::out | std::ios::binary);
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
		//TODO:problems with loading the datapath 
		EntryDataPath dataPath = GetDataPath(entry.mId);

		//copy both entry summary and entry data path into 
		std::memcpy(&entryBuffer[0], &entry.GetSummary().ToBinary()[0], EntryInfo_Short::BYTESIZE);
		std::memcpy(&entryBuffer[0] + EntryInfo_Short::BYTESIZE, &dataPath.ToBinary()[0], EntryDataPath::BYTESIZE);

		entriesFile.write(entryBuffer, ENTRYSTORAGESIZE);
	}
	delete[](entryBuffer);
}

bool EntryDatabase::ReadGameEntryData(const EntryInfo_Short& gameEntrySum, shared_ptr<char[]>& outBinData)
{
	try {
		if (gameEntrySum.type != ET_Game || !DataPathExsists(gameEntrySum.id)) { std::cout << "No data exsists for this entry\n"; throw 001; }

		string filePath = GetGameEntryData_FilePath(gameEntrySum);
		ifstream entriesDataFile = std::ifstream(filePath, std::ios::out | std::ios::binary);

		if (!entriesDataFile.good()) { std::cout << "Failed to read to entry data file: " << filePath << "\n"; throw 001; }

		char* dataBuffer = new char[GameEntry::DATA_BYTESIZE];
		entriesDataFile.read(dataBuffer, GameEntry::DATA_BYTESIZE);

		outBinData = shared_ptr<char[]>(dataBuffer);
		return true;
	}
	catch (int errCode) {
		std::cout << "Error reading entry file data\n";
		return false;
	}	
}

void EntryDatabase::WriteGameEntryData(shared_ptr<GameEntry> _gameEntry, const EntryDataPath& _dataPath)
{
	SetupDir(GetGameEntryData_ParentDirPath(_dataPath));
	SetupDir(GetGameEntryData_DirPath(_dataPath));

	try {
		string filePath = GetGameEntryData_FilePath(_dataPath, _gameEntry->Id(), _gameEntry->Name());
		ofstream entriesDataFile = std::ofstream(filePath, std::ios::out | std::ios::binary);

		if (!entriesDataFile.good()) { std::cout << "Failed to write to entry data file: " << filePath << "\n"; throw; }

		entriesDataFile.write(&_gameEntry->GetBinaryData().get()[0], GameEntry::DATA_BYTESIZE);

		std::cout << _gameEntry->Name() << " - Successfully added to the database\n";
	}
	catch (int errCode) {
		std::cout << "Error writing data to file\n";
	}
}

void EntryDatabase::AddEntry(shared_ptr<Entry> _entry)
{
	if (!IsValidEntry(_entry.get())) { std::cout << "Failed to add entry to database\n"; return; }

	EntryDataPath dataPath = GetDataPath(_entry->mId);

	mActiveEntries.push_back(Entry(*_entry));
	mEntryDataPaths.emplace(_entry->mId, dataPath);

	RemoveTempId(_entry->mId);
	UpdateEntriesFile();
}

void EntryDatabase::AddGameEntry(shared_ptr<GameEntry> _gameEntry)
{
	shared_ptr<Entry> _entry = dynamic_pointer_cast<Entry>(_gameEntry);
	if (!IsValidEntry(_entry.get())) { std::cout << "Failed to add entry to database\n"; return; }
	AddEntry(_entry);

	EntryDataPath dataPath =  GetDataPath(_gameEntry->mId);

	WriteGameEntryData(_gameEntry, dataPath);
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

bool EntryDatabase::IsValidEntry(Entry* _entry)
{
	return (!IsDuplicateEntry(_entry) && _entry->mId != 0);
}

bool EntryDatabase::IsDuplicateEntry(Entry* _entry)
{
	for (auto& curEntry : mActiveEntries) {
		//entry is a duplicate if another entry with the same name and year exsits, checks using custom == operator
		if (*_entry == curEntry) {
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
	if (IsDuplicateEntry(&_entry)) { std::cout << "Entry is duplicate\n"; return false; }

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

inline bool EntryDatabase::DataPathExsists(ENTRYID _entryId)
{
	return mEntryDataPaths.count(_entryId) > 0;
}

EntryDataPath EntryDatabase::GenerateDataPath(ENTRYID _entryId)
{
	if (DataPathExsists(_entryId)) { return mEntryDataPaths[_entryId]; }

	uint16_t parentDirIndex = 1; uint8_t entryDirIndex = 0;
	for (auto entryPathPair : mEntryDataPaths) {

		if (entryPathPair.second.ParentDirIndex >= parentDirIndex) {
			parentDirIndex = entryPathPair.second.ParentDirIndex;

			if ((entryPathPair.second.EntryDirIndex + 255 * entryPathPair.second.ParentDirIndex)
				>= (entryDirIndex + 255 * parentDirIndex)) {
				entryDirIndex = entryPathPair.second.EntryDirIndex;
			}
		}
	}

	if (entryDirIndex == 255) {
		parentDirIndex++;
	}
	entryDirIndex += 1;

	EntryDataPath dataPath = EntryDataPath(true, parentDirIndex, entryDirIndex);
	mEntryDataPaths.emplace(_entryId, dataPath);

	return dataPath;
	//folder name will add zero's to make 5 digits
}

EntryDataPath EntryDatabase::GetDataPath(ENTRYID _entryId)
{
	if (!DataPathExsists(_entryId)) {
		return GenerateDataPath(_entryId);
	}
	return mEntryDataPaths[_entryId];
}

inline string EntryDatabase::GetGameEntryData_ParentDirPath(EntryDataPath dataPath)
{
	//ensure dir name's length is 5
	string parentDir = std::to_string(dataPath.ParentDirIndex);
	int extraZeros = 5 - parentDir.length();
	for (size_t i = 0; i < extraZeros; i++) { parentDir = "0" + parentDir; }

	string path = ENTRIESDATA_DIR_PATH + ENTRIESDATA_GAME_DIRNAME + parentDir + "/";
	return path;
}

inline string EntryDatabase::GetGameEntryData_DirPath(EntryDataPath dataPath)
{
	//ensure entry's name's length is 3
	string entryDir = std::to_string(dataPath.EntryDirIndex);
	int extraZeros = 3 - entryDir.length();
	for (size_t i = 0; i < extraZeros; i++) { entryDir = "0" + entryDir; }

	string path = GetGameEntryData_ParentDirPath(dataPath) + entryDir + "/";
	return path;
}

inline string EntryDatabase::GetGameEntryData_DirPath(ENTRYID gameEntryId)
{
	EntryDataPath dataPath = GetDataPath(gameEntryId);
	return GetGameEntryData_DirPath(dataPath);
}

inline string EntryDatabase::GetGameEntryData_FilePath(EntryDataPath dataPath, ENTRYID _entryId, string _entryName)
{
	return 	GetGameEntryData_DirPath(dataPath) + std::to_string(_entryId) + "-" + _entryName + ".dat";
}

inline string EntryDatabase::GetGameEntryData_FilePath(EntryInfo_Short entrySum)
{
	if (!DataPathExsists(entrySum.id)) { std::cout << "No data found for this entry\n"; throw 001; }

	return GetGameEntryData_FilePath(mEntryDataPaths[entrySum.id], entrySum.id, entrySum.name);
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

