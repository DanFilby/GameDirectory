#include "Entries.h"

#pragma region Entry

Entry::Entry()
{
}

Entry::Entry(ENTRYID _id, EntryType _type, uint16_t _year, string _name)
	:mId(_id), mType(_type), mYear(_year), mName(_name){
}

Entry::~Entry()
{
}

ENTRYID Entry::Id() { return mId; }

string Entry::Name() { return mName; }

EntryInfo_Short Entry::GetSummary()
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, ET_Game, mYear, mName };

	return info;
}

unique_ptr<char[]> Entry::GetRawData_Short()
{
	return nullptr;
}

#pragma endregion


#pragma region GameEntry

GameEntry::GameEntry()
{
	mId = 37;
	mYear = 2023;
	mName = "Penguin Village";
}

GameEntry::GameEntry(char* rawData)
{
}

GameEntry::~GameEntry()
{
}

EntryInfo_Short GameEntry::GetSummary()
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, ET_Game, mYear, mName };
	return info;
}

unique_ptr<char[]> GameEntry::GetRawData_Short()
{
	//init memory to store the raw data - 32 Bytes
	unique_ptr<char[]> gameDat = unique_ptr<char[]>(new char[EntryInfo_Short::BYTESIZE]);

	//get game entry's info
	EntryInfo_Short info = GetSummary();

	//copy the game info's data into the gameDat pointer
	std::memcpy(gameDat.get(), info.ToBinary().get(), EntryInfo_Short::BYTESIZE);

	return gameDat;
}



#pragma endregion



