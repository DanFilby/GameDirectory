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
	std::map<EntryType, int> entryTypeMap{ {T_Base, 0},{T_Game, 0}, {T_Studio, 0 } };
	
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

	//loop over file reading each entree and add to active entries

}

void EntryDatabase::WriteEntries()
{
	ofstream entriesFile = std::ofstream(DIR_PATH + ENTRIESLIST_FNAME, std::ios::out | std::ios::binary);

	//write header

	//16 byte header
	uint16_t* header = new uint16_t[8];

	header[0] = (uint16_t)GetEntryCount();




	//write the frame's header to file
	//mWriteFileStream.write((char*)header, mFrameHeaderSize * sizeof(uint64_t));

	//loops over entries writing each to file

}