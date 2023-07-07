#include "EntryBuilders.h"

EntryBuilder::EntryBuilder(shared_ptr<EntryDatabase> _entryDatabase)
	:mEntryDatabase(_entryDatabase) {
	//'mEntryDatabase = shared_ptr<EntryDatabase>(_entryDatabase);
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
	
	//set id to 0, as it's not being added to the database right away 
	outputEntry.mId = 0;

	//set the returned out entry 
	entry = make_shared<Entry>(outputEntry);

	return true;
}

bool EntryBuilder::BuildAndSaveEntry(shared_ptr<Entry>& entry)
{
	//build the entry normally, if valid set id and save to database
	if (BuildEntry(entry)) {
		//get and set a unique id for this entry
		bool databaseCheck = mEntryDatabase->SetUniqueId(*entry.get());

		//return if failed database checks
		if (!databaseCheck) { std::cout << "Entry failed database checks\n"; return false; }

		//add entry to database, and save to file
		mEntryDatabase->AddEntry(*entry.get());
		mEntryDatabase->UpdateEntriesFile();

		return true;
	}
	return false;
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
	SetNameYear(info.name, info.year);
	mCurrentEntry->mId = info.id;
}

void EntryBuilder::SetNameYear(string _name, uint16_t _year)
{
	SetName(_name);
	SetYear(_year);
}

void EntryBuilder::SetName(string _name)
{
	//check valid name
	if (mCurrentEntry->IsValid_Name(_name)) {
		mCurrentEntry->mName = _name;
	}
	else { std::cout << "Invalid name given to entry\n"; }
}

void EntryBuilder::SetYear(uint16_t _year)
{
	//check valid year
	if (mCurrentEntry->IsValid_Year(_year)) {
		mCurrentEntry->mYear = _year;
	}
	else { std::cout << "Invalid year given to entry\n"; }
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
