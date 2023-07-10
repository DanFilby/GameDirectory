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

EntryInfo_Short Entry::GetSummary()
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, mType, mYear, mName };

	return info;
}

unique_ptr<char[]> Entry::GetRawData_Short()
{
	return GetSummary().ToBinary();
}

bool Entry::operator==(const Entry& compareEntry)
{
	if ((strcmp(this->mName.c_str(), compareEntry.mName.c_str()) == 0 && this->mYear == compareEntry.mYear
		&& this->mType == compareEntry.mType )|| this->mId == compareEntry.mId) {
		return true;
	}
	return false;
}

bool Entry::IsValid_Name(const string& _name)
{
	if (_name.length() >= MINLEN_ENTRYNAME && _name.length() <= MAXLEN_ENTRYNAME) {
		return true;
	}
	return false;
}

bool Entry::IsValid_Year(const uint16_t _year)
{
	if (_year >= 1950 && _year <= 2030) {
		return true;
	}
	return false;
}

#pragma endregion


#pragma region GameEntry

GameEntry::GameEntry()
{
	mId = 37;
	mYear = 2023;
	mName = "Penguin Village";
}

GameEntry::GameEntry(ENTRYID _id, uint16_t _year, string _name)
{
	mId = _id;
	mType = ET_Game;
	mYear = _year;
	mName = _name;

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

void GameEntry::SetShortDescription(string _shortDescription)
{
	_shortDescription.resize(SHORTDESCRIPTION_MAXLEN);
	mShortDescription = _shortDescription;
}

void GameEntry::SetFullDescription(string _fullDescription)
{
	_fullDescription.resize(FULLDESCRIPTION_MAXLEN);
	mShortDescription = _fullDescription;
}



#pragma endregion



