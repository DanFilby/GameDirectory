#include "EntryBuilders.h"

EntryBuilder::EntryBuilder(EntryDatabase* _entryDatabase)
	:mEntryDatabase() {
	ClearBuild();
}

shared_ptr<Entry> EntryBuilder::BuildEntry()
{
	//check everything has been built, name etc then assign id

	//create a copy of the temp entry
	Entry outputEntry = Entry(*mCurrentEntry);

	mEntryDatabase->AddEntry(outputEntry);

	return shared_ptr<Entry>();
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


