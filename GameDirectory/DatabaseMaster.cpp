#include "Common.h"

#include "DatabaseMaster.h"

void DatabaseMaster::AppInit()
{
	//check main directory valid, if not setup
	SetupDir(APP_DATA_DIR_PATH);

	mEntryDatabase = nullptr;

	//init all databases, each check their own respective directories 
	mGame_GenreDatabase = make_shared<GenreListDataBase>();
	mGame_TagDatabase = make_shared<TagListDataBase>();

	mEntryDatabase = make_shared<EntryDatabase>(mGame_GenreDatabase, mGame_TagDatabase);
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
