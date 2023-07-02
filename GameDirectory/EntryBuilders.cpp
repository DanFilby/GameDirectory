#include "EntryBuilders.h"

EntryBuilder::EntryBuilder(EntryDatabase* _entryDatabase)
	:mEntryDatabase(_entryDatabase) {
	ClearBuild();
	srand(time(NULL));
}

bool EntryBuilder::BuildEntry(shared_ptr<Entry>& entry)
{
	//check all required entry fields are filled
	if (!RequiredFieldsCheck()) {
		std::cout << "Required entry are fields incomplete\n";
		return false;
	}

	//create a copy of the temp entry
	Entry outputEntry = Entry(*mCurrentEntry);
	
	//get and set a unique id for this entry
	bool databaseCheck = mEntryDatabase->SetUniqueId(outputEntry);

	//return if failed database checks
	if (!databaseCheck) { std::cout << "Entry failed database checks\n"; return false; }

	//set the returned out entry 
	entry = make_shared<Entry>(outputEntry);

	//add entry to database
	mEntryDatabase->AddEntry(outputEntry);

	return true;
}

void EntryBuilder::ClearBuild()
{
	if (mCurrentEntry) { delete(mCurrentEntry); }
	mCurrentEntry = new Entry;
	mCurrentEntry->mType = ET_Base;
	mCurrentEntry->mYear = 0;
	mCurrentEntry->mName.clear();
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
	//check if year and name have been assigned
	if (mCurrentEntry->mYear == 0) {
		std::cout << "Entry missing required field: " << "Year\n";
		return false;
	}
	if (mCurrentEntry->mName.empty()) {
		std::cout << "Entry missing required field: " << "Name\n";
		return false;
	}
	return true;
}

bool GameEntryBuilder::RequiredFieldsCheck()
{
	if (!EntryBuilder::RequiredFieldsCheck()) {
		std::cout << "Base requirements check failed\n";
		return false;
	}


	return false;
}
