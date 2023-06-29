#pragma once
//builder for video game has a studio step so can setup multiple studios

#include "EntryDatabase.h"

class EntryBuilder {

public:
	EntryBuilder(EntryDatabase* _entryDatabase);

	virtual void SetInfo(EntryInfo_Short info);

	virtual Entry BuildEntry();
	virtual void SaveEntry();

protected:
	EntryDatabase* mEntryDatabase;

};

class GameEntryBuilder : EntryBuilder {

public:
	GameEntryBuilder();

	Entry BuildEntry();


};