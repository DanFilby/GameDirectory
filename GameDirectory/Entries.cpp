#include "Entries.h"

#pragma region Entry

Entry::Entry()
{
}

Entry::~Entry()
{
}

ENTRYID Entry::Id() { return mId; }

string Entry::Name() { return mName; }

EntryInfo_Short Entry::GetInfoShort()
{
	return NULL;
}

unique_ptr<char[]> Entry::ToFile()
{
	return nullptr;
}

#pragma endregion


#pragma region GameEntry

GameEntry::GameEntry()
{
	mId = 37;
	mName = "Penguin Village";
}

GameEntry::GameEntry(char* rawData)
{
}

GameEntry::~GameEntry()
{
}

EntryInfo_Short GameEntry::GetInfoShort()
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ this->mId, T_Game, mName };

	return info;
}

unique_ptr<char[]> GameEntry::ToFile()
{
	//init memory to store the raw data - 32 Bytes
	unique_ptr<char[]> gameDat = unique_ptr<char[]>(new char[EntryInfo_Short::BYTESIZE]);

	//get game entry's info
	EntryInfo_Short info = GetInfoShort();

	//copy the game info's data into the gameDat pointer
	std::memcpy(gameDat.get(), info.ToBinary().get(), EntryInfo_Short::BYTESIZE);

	return gameDat;
}

int GameEntry::Rating()
{
	return 4;
}

#pragma endregion



