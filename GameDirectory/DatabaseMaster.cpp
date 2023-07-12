#include "DatabaseMaster.h"

void DatabaseMaster::AppInit()
{
	//check main directory valid, if not setup
	SetupDir(DIR_PATH);

	mEntryDatabase = nullptr;

	//init all databases, each check their own respective directories 
	mEntryDatabase = make_shared<EntryDatabase>();
	mGame_GenreDatabase = make_shared<GenreListDataBase>();
	mGame_TagDatabase = make_shared<TagListDataBase>();
}

void DatabaseMaster::Close()
{
}

void DatabaseMaster::CheckDatabases()
{
	mEntryDatabase->FileDirectoriesCheck();
	mGame_GenreDatabase->FileDirectoriesCheck();
	mGame_TagDatabase->FileDirectoriesCheck();
}
