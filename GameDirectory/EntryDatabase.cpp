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
	fstream entriesFile(DIR_PATH + ENTRIESLIST_FNAME);

	//check file exsists and isn't empty
	if (!entriesFile.good()) {
		//create file
		std::cout << "file don't exsist\n\n";
		ofstream(DIR_PATH + ENTRIESLIST_FNAME);
	}
	else if (entriesFile.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "file is empty\n\n";
	}


	//read header

	//loop over file reading each entree and add to active entries

}
