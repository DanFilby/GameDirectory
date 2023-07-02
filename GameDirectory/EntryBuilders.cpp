#include "EntryBuilders.h"

EntryBuilder::EntryBuilder(EntryDatabase* _entryDatabase)
	:mEntryDatabase(_entryDatabase) {
	ClearBuild();
	srand(time(NULL));
}

bool EntryBuilder::BuildEntry(shared_ptr<Entry>& entry)
{
	//check everything has been built, name etc then assign id

	//create a copy of the temp entry
	Entry outputEntry = Entry(*mCurrentEntry);
	
	//get a unique id for this entry and set it
	mEntryDatabase->SetUniqueId(outputEntry);

	//set output entry 
	entry = make_shared<Entry>(outputEntry);

	//add entry to database
	//mEntryDatabase->AddEntry(outputEntry);

	return true;
}

void EntryBuilder::ClearBuild()
{
	if (mCurrentEntry) { delete(mCurrentEntry); }
	mCurrentEntry = new Entry;
	mCurrentEntry->mType = ET_Base;
}

void EntryBuilder::SetInfo(EntryInfo_Short info)
{

}

void EntryBuilder::SetNameYear(string _name, uint16_t _year)
{
	SetName(_name);
	SetYear(_year);
}

void EntryBuilder::SetName(string _name)
{
	//TODO: verification check
	mCurrentEntry->mName = _name;
}

void EntryBuilder::SetYear(uint16_t _year)
{
	//TODO: verification check
	mCurrentEntry->mYear = _year;
}

bool EntryBuilder::RequiredFieldsCheck()
{



	return false;
}

bool GameEntryBuilder::RequiredFieldsCheck()
{
	bool failed = EntryBuilder::RequiredFieldsCheck();


	return false;
}
