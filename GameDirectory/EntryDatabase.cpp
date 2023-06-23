#include "EntryDatabase.h"

EntryDatabase::EntryDatabase()
{
	//setup files, store in folder ids and database

	FolderSetup();

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
