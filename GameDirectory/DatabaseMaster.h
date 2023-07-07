#pragma once
#include "Database.h"
#include "EntryDatabase.h"
#include "EntryUtils.h"


/// <summary>
/// Database manager for the whole application, checks any neccessary directories on start-up
/// </summary>
class DatabaseManager : Database {
public:
	/// <summary>
	/// Inits databases for the entire application, validation checks
	/// </summary>
	void AppInit();

	void Close();
};