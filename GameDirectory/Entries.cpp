#include "Entries.h"

#pragma region Entry

Entry::Entry()
{
}

Entry::~Entry()
{
}

ENTRYID Entry::ID() { return mId; }

string Entry::NAME() { return mName; }

char* Entry::ToFile()
{
	return nullptr;
}

#pragma endregion


#pragma region GameEntry

GameEntry::GameEntry()
{
	mId = 37;

}

GameEntry::GameEntry(char* rawData)
{
}

GameEntry::~GameEntry()
{
}

char* GameEntry::ToFile()
{
	return nullptr;
}

int GameEntry::Rating()
{
	return 4;
}

#pragma endregion



