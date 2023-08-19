#include "Common.h"

#include "EntryDatabase.h"

EntryDatabase::EntryDatabase(shared_ptr<GenreListDataBase> _genreDatabase, shared_ptr<TagListDataBase> _tagDatabase)
	:mGenreDatabase(_genreDatabase), mTagDatabase(_tagDatabase), mEntryRelationsFileManager(mEntryDataPaths, GetEntryData_DirPath)
{
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
	assert(IsDirVaild(APP_DATA_DIR_PATH));

	SetupDir(ENTRIESDATA_DIR_PATH);
	SetupDir(ENTRIESDATA_DIR_PATH + ENTRIESDATA_GAME_DIRNAME);
}

void EntryDatabase::LoadEntriesFile()
{
	FileReadCheck(APP_DATA_DIR_PATH + ENTRIESLIST_FNAME);

	fstream entriesFile(APP_DATA_DIR_PATH + ENTRIESLIST_FNAME, std::ios::in | std::ios::binary);

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
	}
}

void EntryDatabase::UpdateEntriesFile()
{
	ofstream entriesFile = std::ofstream(APP_DATA_DIR_PATH + ENTRIESLIST_FNAME, std::ios::out | std::ios::binary);
	if (!entriesFile.good()) { std::cout << "Failed to write to file: " << APP_DATA_DIR_PATH + ENTRIESLIST_FNAME << "\n"; return; }

	//generate the file's header, containing number of each entry stored
	EntryFileHeader entriesHeader = EntryFileHeader(GetEntryTypeCount());

	//write a header to the entry file to hold the counts of each entry stored
	entriesFile.write((char*)entriesHeader.ToBinary().get(), EntryFileHeader::BYTESIZE);

	//write twice to total 16 bytes and for validation
	entriesFile.write((char*)entriesHeader.ToBinary().get(), EntryFileHeader::BYTESIZE);

	char* entryBuffer = new char[ENTRYSTORAGESIZE];

	//write each entry to the file. each are 36 bytes
	for (const auto& entry : mActiveEntries) {
		EntryDataPath dataPath = GetDataPath(entry.mId);

		//copy both entry summary and entry data path into 
		std::memcpy(&entryBuffer[0], &entry.GetSummary().ToBinary()[0], EntryInfo_Short::BYTESIZE);
		std::memcpy(&entryBuffer[0] + EntryInfo_Short::BYTESIZE, &dataPath.ToBinary()[0], EntryDataPath::BYTESIZE);

		entriesFile.write(entryBuffer, ENTRYSTORAGESIZE);
	}
	delete[](entryBuffer);
}

template<typename EntryTy>
bool EntryDatabase::ReadEntryData(const EntryInfo_Short& entrySum, shared_ptr<char[]>& outBinData)
{
	try {
		if (!DataPathExsists(entrySum.id)) { std::cout << "No data exsists for this entry\n"; throw 001; }

		string filePath = GetEntryData_FilePath(entrySum);
		ifstream entriesDataFile = std::ifstream(filePath, std::ios::out | std::ios::binary);

		if (!entriesDataFile.good()) { std::cout << "Failed to read to entry data file: " << filePath << "\n"; throw 001; }

		char* dataBuffer = new char[EntryTy::DATA_BYTESIZE];
		entriesDataFile.read(dataBuffer, EntryTy::DATA_BYTESIZE);

		outBinData = shared_ptr<char[]>(dataBuffer);
		return true;
	}
	catch (int errCode) {
		std::cout << "Error reading entry file data\n";
		return false;
	}	
}

void EntryDatabase::WriteEntryData(shared_ptr<Entry> _Entry, const EntryDataPath& _dataPath)
{
	SetupDir(GetEntryData_ParentDirPath(_dataPath, _Entry->Type()));
	SetupDir(GetEntryData_DirPath(_dataPath, _Entry->Type()));

	try {
		string filePath = GetEntryData_FilePath(_dataPath, _Entry->Id(), _Entry->Name());
		ofstream entriesDataFile = std::ofstream(filePath, std::ios::out | std::ios::binary);

		if (!entriesDataFile.good() || _Entry->GetDataByteSize() <= 0) { std::cout << "Failed to write to entry data file: " << filePath << "\n"; throw; }

		entriesDataFile.write(&_Entry->GetBinaryData().get()[0], _Entry->GetDataByteSize());

		std::cout << _Entry->Name() << " - Successfully added entry data to the database\n";

		WriteRelations(_Entry);

		std::cout << _Entry->Name() << " - Successfully added entry relations to the database\n";
		entriesDataFile.close();
	}
	catch (int errCode) {
		std::cout << "Error writing data to file\n";
	}

}

void EntryDatabase::WriteRelations(shared_ptr<Entry> _Entry)
{
	for (auto relation : _Entry->GetRelations()) {
		mEntryRelationsFileManager.Write_EntryRelationFile(_Entry->GetSummary(), *relation);
	}
}

template<typename EntryTy>
void EntryDatabase::ReadRelationsIntoEntry(shared_ptr<EntryTy> _Entry)
{
	try {
		shared_ptr<Entry> entry = dynamic_pointer_cast<Entry>(_Entry);

		for (EntryRelations* relation : entry->GetRelations()) {
			EntryRelations readInRelation = mEntryRelationsFileManager.Read_EntryRelationFile(entry->GetSummary(), relation->relationType);
			*relation = EntryRelations(readInRelation);
		}
	}
	catch (int errCode) {
		std::cout << "Error reading game entry relations\n";
	}
}

void EntryDatabase::AddEntry(shared_ptr<Entry> entry)
{
	if (!IsValidEntry(entry.get())) { std::cout << "Failed to add entry to database\n"; return; }

	EntryDataPath dataPath = GetDataPath(entry->mId);

	mActiveEntries.push_back(Entry(*entry));
	mEntryDataPaths.emplace(entry->mId, dataPath);

	RemoveTempId(entry->mId);
	UpdateEntriesFile();

	WriteEntryData(entry, dataPath);
}

void EntryDatabase::UpdateEntryData(shared_ptr<Entry> _entry)
{
	if (!EntryExsists(_entry->Id())) { return; }

	EntryDataPath dataPath = GetDataPath(_entry->mId);

	WriteEntryData(_entry, dataPath);
}

bool EntryDatabase::GetEntryAndData(ENTRYID _entryId, shared_ptr<Entry>& outEntry)
{
	if (!EntryExsists(_entryId)) { return false; }

	switch (GetEntryType(_entryId)) {
	case(ET_Game):
		outEntry = dynamic_pointer_cast<Entry>(GetGameEntry(_entryId)); break;
	case(ET_Studio):
		outEntry = dynamic_pointer_cast<Entry>(GetStudioEntry(_entryId)); break;
	default:
		outEntry = shared_ptr<Entry>(nullptr); break;
	}

	if (outEntry.get() != nullptr) { return true; }
	else { return false; }
}

void EntryDatabase::RemoveEntry(ENTRYID entryId)
{
	if (!EntryExsists(entryId)) { return; }

	if (DataPathExsists(entryId)) {
		std::filesystem::remove_all(GetEntryData_DirPath(entryId));
	}

	mActiveEntries.erase(mActiveEntries.begin() + GetEntryListIndex(entryId));
	mEntryDataPaths.erase(entryId);

	UpdateEntriesFile();
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
	//TODO: differences for game and studio entries

	if (DataPathExsists(_entryId)) { return mEntryDataPaths[_entryId]; }

	uint16_t parentDirIndex = 1; uint8_t entryDirIndex = 0;
	for (const auto& entryPathPair : mEntryDataPaths) {

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

string EntryDatabase::GetParentDirNameFromType(const EntryType entryType)
{
	switch (entryType)
	{
	case(ET_Game):
		return ENTRIESDATA_GAME_DIRNAME;
	case(ET_Studio):
		return ENTRIESDATA_STUDIO_DIRNAME;
	default:
		return "ERROR-DIR";
	}
}

string EntryDatabase::GetEntryData_ParentDirPath(const EntryDataPath dataPath, const EntryType entryType)
{
	//ensure dir name's length is 5
	string parentDir = std::to_string(dataPath.ParentDirIndex);
	int extraZeros = 5 - parentDir.length();
	for (size_t i = 0; i < extraZeros; i++) { parentDir = "0" + parentDir; }

	string path = ENTRIESDATA_DIR_PATH + GetParentDirNameFromType(entryType) + parentDir + "/";
	return path;
}

string EntryDatabase::GetEntryData_DirPath(const EntryDataPath dataPath, const EntryType entryType)
{
	//ensure entry's name's length is 3
	string entryDir = std::to_string(dataPath.EntryDirIndex);
	int extraZeros = 3 - entryDir.length();
	for (size_t i = 0; i < extraZeros; i++) { entryDir = "0" + entryDir; }

	string path = GetEntryData_ParentDirPath(dataPath, entryType) + entryDir + "/";
	return path;
}

string EntryDatabase::GetEntryData_DirPath(ENTRYID entryId)
{
	EntryDataPath dataPath = GetDataPath(entryId);
	return GetEntryData_DirPath(dataPath, GetEntryType(entryId));
}

string EntryDatabase::GetEntryData_FilePath(EntryDataPath dataPath, ENTRYID _entryId, string _entryName)
{
	return 	GetEntryData_DirPath(dataPath, GetEntryType(_entryId)) + std::to_string(_entryId) + "-" + _entryName + ".dat";
}

string EntryDatabase::GetEntryData_FilePath(EntryInfo_Short entrySum)
{
	if (!DataPathExsists(entrySum.id)) { std::cout << "No data found for this entry\n"; throw 001; }

	return GetEntryData_FilePath(mEntryDataPaths[entrySum.id], entrySum.id, entrySum.name);
}

shared_ptr<GameEntry> EntryDatabase::GetGameEntry(ENTRYID _id)
{
	if (!EntryExsists(_id)) { std::cout << "Game entry does not exsist\n"; return shared_ptr<GameEntry>(nullptr); }

	shared_ptr<char[]> entryData;
	if (ReadEntryData<GameEntry>(GetEntrySummary(_id), entryData)) {

		shared_ptr<GameEntry> gameEntry = make_shared<GameEntry>(GetEntrySummary(_id), entryData, mGenreDatabase, mTagDatabase);
		ReadRelationsIntoEntry<GameEntry>(gameEntry);

		return gameEntry;
	}
	else { return shared_ptr<GameEntry>(nullptr); }
}

shared_ptr<StudioEntry> EntryDatabase::GetStudioEntry(ENTRYID _id)
{
	if (!EntryExsists(_id)) { std::cout << "Stuido entry does not exsist\n"; return shared_ptr<StudioEntry>(nullptr); }

	shared_ptr<char[]> entryData;
	if (ReadEntryData<StudioEntry>(GetEntrySummary(_id), entryData)) {

		shared_ptr<StudioEntry> studioEntry = make_shared<StudioEntry>(GetEntrySummary(_id), entryData);
		ReadRelationsIntoEntry<StudioEntry>(studioEntry);

		return studioEntry;
	}
	else { return shared_ptr<StudioEntry>(nullptr); }
}

Entry EntryDatabase::GetBaseEntry(ENTRYID _id)
{
	for (auto& entry : mActiveEntries) {
		if (entry.mId == _id) { return entry; }
	}

	std::cout << "Unable to find entry in database\n";
	return Entry();
}

EntryInfo_Short EntryDatabase::GetEntrySummary(ENTRYID _id)
{
	return GetBaseEntry(_id).GetSummary();
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

EntryType EntryDatabase::GetEntryType(ENTRYID _id)
{
	return GetEntrySummary(_id).type;
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

void EntryDatabase::PrintEntriesPaths()
{
	for (auto entryPathPair : mEntryDataPaths) {
		std::cout << "Id: " << entryPathPair.first << " | " << GetEntrySummary(entryPathPair.first).name
			<<" | DataPath: " << entryPathPair.second.EntryHasData << "  "
			<< entryPathPair.second.ParentDirIndex << "  " << (int)entryPathPair.second.EntryDirIndex << "\n";
	}
}

int EntryDatabase::GetEntryListIndex(ENTRYID _id)
{
	int entryIndex = -1;
	for (size_t i = 0; i < mActiveEntries.size(); i++)
	{
		if (mActiveEntries[i].mId == _id) {
			entryIndex = i;
			break;
		}
	}
	return entryIndex;
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

FileManager_EntryRelations::FileManager_EntryRelations(const map<ENTRYID, EntryDataPath>& _dataPathsMap, string(*getEntryDataPath)(const EntryDataPath dataPath, const EntryType entryType))
	:mDataPathsMap(_dataPathsMap), mGetEntryDataPath(getEntryDataPath)
{
}

void FileManager_EntryRelations::Write_EntryRelationFile(EntryInfo_Short entrySum, EntryRelations entryRelations)
{
	vector<ENTRYID> relations = entryRelations.relations;
	EnsureRelationsSizeValid(relations, entryRelations.relationType);
	uint16_t relationCount = relations.size();

	try {
		fstream entryRelationsFile{};
		GetEntryRelationsFile(entryRelationsFile, entrySum, entryRelations.relationType);

		WriteFileHeader(entryRelationsFile, relationCount, entryRelations.relationType);
		WriteFileBody(entryRelationsFile, relations);

		entryRelationsFile.close();
	}
	catch (int errCode) {
		std::cout << "Error: " << errCode << " - Couldn't write entry relations file. Id: " << entrySum.id << "\n";
	}	
}

EntryRelations FileManager_EntryRelations::Read_EntryRelationFile(EntryInfo_Short entrySum, EntryRelationsType relationType)
{
	try {
		EntryRelations entryRelations(relationType);

		fstream entryRelationsFile{};
		GetEntryRelationsFile(entryRelationsFile, entrySum, entryRelations.relationType);

		uint16_t relationCount;
		ReadFileHeader(entryRelationsFile, relationCount, relationType);

		ENTRYID* readBuffer = new ENTRYID;

		for (size_t i = 0; i < relationCount; i++)
		{
			entryRelationsFile.read((char*)readBuffer, sizeof(ENTRYID));
			entryRelations.relations.push_back(ENTRYID(*readBuffer));
		}

		return entryRelations;
	}
	catch (int errCode) {
		std::cout << "Error: " << errCode << " - Couldn't read entry relations file. Id: " << entrySum.id << "\n";
	}
}


inline void FileManager_EntryRelations::WriteFileHeader(fstream& file, const uint16_t& relationCount, const EntryRelationsType& type)
{
	//write 4-byte header -> (relation count) (relation type)
	char* writeBuffer = new char[4];
	memcpy(&writeBuffer[0], &relationCount, 2);
	memcpy(&writeBuffer[2], &type, 2);
	file.write(&writeBuffer[0], 4);
	delete[](writeBuffer);
}

inline void FileManager_EntryRelations::ReadFileHeader(fstream& file, uint16_t& relationCount, EntryRelationsType& type)
{
	char* readBuffer = new char[4];
	file.read(&readBuffer[0], 4);
	memcpy(&relationCount, &readBuffer[0], 2);
	memcpy(&type, &readBuffer[2], 2);
	delete[](readBuffer);
}

void FileManager_EntryRelations::WriteFileBody(fstream& file, const vector<ENTRYID>& relations)
{
	char* writeBuffer = new char[sizeof(ENTRYID)];

	for (const ENTRYID& relation : relations) {
		std::memcpy(&writeBuffer[0], &relation, sizeof(ENTRYID));
		file.write(&writeBuffer[0], sizeof(ENTRYID));
	}

	delete[](writeBuffer);
}

inline void FileManager_EntryRelations::EnsureRelationsSizeValid(vector<ENTRYID>& relations, EntryRelationsType relationsType)
{
	uint16_t maxCount = EntryRelations::MaxRelationsCount(relationsType);

	if (relations.size() > maxCount) { relations.resize(maxCount); }
}

inline string FileManager_EntryRelations::RelationsFilePath(string entryDirPath, ENTRYID entryId, EntryRelationsType type)
{
	return entryDirPath + std::to_string(entryId) + "-" + EntryRelations::RelationTypeToString(type) + "-Relations.dat";
}

void FileManager_EntryRelations::GetEntryRelationsFile(fstream& entryRelationsFile, EntryInfo_Short entrySum, EntryRelationsType relationsType)
{
	if (mDataPathsMap.count(entrySum.id) != 1) { throw 002; }
	EntryDataPath entryDataPath = mDataPathsMap.at(entrySum.id);

	string entryDataDir = mGetEntryDataPath(entryDataPath, entrySum.type);
	string filePath = RelationsFilePath(entryDataDir, entrySum.id, relationsType);

	//entryRelationsFile = fstream(filePath, std::ios::binary | std::ios::in | std::ios::out);
	entryRelationsFile.open(filePath, std::ios::binary | std::ios::out | std::ios::in);

	if (!entryRelationsFile) { 
		//file doesn't exsist, creating file
		ofstream createFileStream(filePath, std::ios::out | std::ios::binary);	
		createFileStream.close();

		entryRelationsFile.open(filePath, std::ios::binary | std::ios::out | std::ios::in);
	}

	if (entryRelationsFile.bad()) { throw 002; }
}