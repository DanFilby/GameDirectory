#pragma once
#include "Database.h"
#include "EntryDatabase.h"
#include "EntryUtils.h"


/// <summary>
/// Database manager for the whole application, checks any neccessary directories on start-up
/// </summary>
class DatabaseMaster : Database {
public:
	/// <summary>
	/// Inits databases for the entire application
	/// </summary>
	void AppInit();
	void Close();

	/// <summary>
	/// checks all file directories are valid
	/// </summary>
	void CheckDatabases();

	shared_ptr<EntryDatabase> GetEntryDatabase() { return mEntryDatabase; }
	shared_ptr<GenreListDataBase> GetGenreDatabase() { return mGame_GenreDatabase; }
	shared_ptr<TagListDataBase> GetTagDatabase() { return mGame_TagDatabase; }

private:
	shared_ptr<EntryDatabase> mEntryDatabase;
	shared_ptr<GenreListDataBase> mGame_GenreDatabase;
	shared_ptr<TagListDataBase> mGame_TagDatabase;

};