#pragma once
//builder for video game has a studio step so can setup multiple studios

#include "EntryDatabase.h"

class EntryBuilder {

public:
	EntryBuilder(EntryDatabase* _entryDatabase);

	virtual shared_ptr<Entry> BuildEntry();
	virtual void ClearBuild();

	virtual void SetInfo(EntryInfo_Short info);
	void SetNameYear(string _name, uint16_t year);
	void SetName(string _name);
	void SetYear(uint16_t _year);


protected:
	EntryDatabase* mEntryDatabase;

private:
	//temp entry which is built through wrapper functions
	Entry* mCurrentEntry;

};

class GameEntryBuilder : EntryBuilder {

public:
	GameEntryBuilder(EntryDatabase* _entryDatabase);

	shared_ptr<Entry> BuildEntry();


};