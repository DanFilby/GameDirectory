#include "EntryDatabase.h"

EntryDatabase::EntryDatabase()
{
	//setup files, store in folder ids and database

	//checks neccessary folders are valid, otherwise creates them
	FolderSetup();

	//reads entries file, loads all saved entry summaries into memory
	LoadEntries();
		
}

EntryDatabase::~EntryDatabase()
{
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

void EntryDatabase::FolderSetup()
{
	//check data directory
	CheckFolder(DIR_PATH);
	
	//checks the entrees data dir 
	CheckFolder(ENTRIESDATA_DIR_PATH);

	std::cout << "\n";
}

void EntryDatabase::CheckFolder(string folderPath)
{
	//convert to windows string format
	std::wstring temp = std::wstring(folderPath.begin(), folderPath.end());
	LPCWSTR wideString = temp.c_str();

	//folder is either created or already exsists
	if (CreateDirectory(wideString, NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError()) {
		std::cout << folderPath << " - Setup\n";
	}
	else { std::cout << "Failed to create Data Folder\n"; }
}


void EntryDatabase::LoadEntries()
{
	//check if file is valid
	fstream entriesFile(DIR_PATH + ENTRIESLIST_FNAME, std::ios::in | std::ios::binary);

	//check file exsists
	if (!entriesFile.good()) {
		//create file
		std::cout << "file don't exsist\n\n";
		ofstream(DIR_PATH + ENTRIESLIST_FNAME);
		return;
	}
	//check the file isn't empty
	else if (entriesFile.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "file is empty\n\n";
		return;
	}

	//read header
	uint16_t* fileHeader = new uint16_t[8];
	entriesFile.read((char*)fileHeader, ENTRIESHEADER_BYTESIZE * 2);
	EntryFileHeader entriesHeader = EntryFileHeader(fileHeader);

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
