#pragma once
//builder for video game has a studio step so can setup multiple studios

#include "EntryDatabase.h"

class EntryBuilder {

public:
	EntryBuilder(shared_ptr<EntryDatabase> _entryDatabase);

	virtual bool BuildEntry(shared_ptr<Entry> & entry);
	virtual bool BuildAndSaveEntry(shared_ptr<Entry>& entry);
	virtual void ClearBuild();

	virtual void SetInfo(EntryInfo_Short info);
	void SetNameYear(string _name, uint16_t year);
	void SetName(string _name);
	void SetYear(uint16_t _year);


protected:
	/// <summary>
	/// checks all required fields for an entry
	/// </summary>
	/// <returns> true if rfequired fields are valid </returns>
	virtual bool RequiredFieldsCheck();

	shared_ptr<EntryDatabase> mEntryDatabase;

private:
	//temp entry which is built through wrapper functions
	Entry* mCurrentEntry;
};

class GameEntryBuilder : EntryBuilder {

public:
	GameEntryBuilder(EntryDatabase* _entryDatabase);

	bool BuildEntry(shared_ptr<Entry>& entry);
	bool BuildGameEntry(shared_ptr<GameEntry>& gameEntry);

private:
	bool RequiredFieldsCheck();

};


//TODO: edit exsisting entry