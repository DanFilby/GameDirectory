#include "Entries.h"

#pragma region Entry

Entry::Entry()
{
	mId = 0;
	mType = ET_Base;
	mYear = 0;
	mName.clear();
}

Entry::Entry(EntryInfo_Short _entrySummary)
	:mId(_entrySummary.id), mType(_entrySummary.type), mYear(_entrySummary.year), mName(_entrySummary.name) {
}

Entry::Entry(ENTRYID _id, EntryType _type, uint16_t _year, string _name)
	:mId(_id), mType(_type), mYear(_year), mName(_name){
}

Entry::~Entry()
{
}

bool Entry::operator==(const Entry& compareEntry) const
{
	if ((strcmp(this->mName.c_str(), compareEntry.mName.c_str()) == 0 && this->mYear == compareEntry.mYear
		&& this->mType == compareEntry.mType) || this->mId == compareEntry.mId) {
		return true;
	}
	return false;
}

EntryInfo_Short Entry::GetSummary() const
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, mType, mYear, mName };

	return info;
}

shared_ptr<char[]> Entry::GetRawData_Short()
{
	return GetSummary().ToBinary();
}

shared_ptr<char[]> Entry::GetRawData()
{
	return GetSummary().ToBinary();
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
	:mGenres(), mTags(), mRatings()
{
	mId = 0;
	mType = ET_Game;
	mYear = 0;
	mName = "";
}

GameEntry::GameEntry(ENTRYID _id, uint16_t _year, string _name)
	:mGenres(), mTags(), mRatings()
{
	mId = _id;
	mType = ET_Game;
	mYear = _year;
	mName = _name;
}

GameEntry::GameEntry(shared_ptr<char[]> rawData)
	:mGenres(), mTags(), mRatings()
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

shared_ptr<char[]> GameEntry::GetRawData()
{
	shared_ptr<char[]> data;

	return shared_ptr<char[]>();
}


void GameEntry::PrintInfo()
{
	std::cout << "\nGame entry: " << mName << "\n";
	std::cout << " Id: " << mId << " Year: " << mYear << "\n";
	std::cout << "- " << mShortDescription << " -\n\n";
	std::cout << "Tags: | " << mTags.GetTagsOneLine() << "\n";
	std::cout << "Genres: | " << mGenres.GetGenresOneLine() << "\n\n";
	std::cout << mFullDescription << "\n\n";
	mRatings.DisplayAllRatings();
}

void GameEntry::SetShortDescription(string _shortDescription)
{
	_shortDescription.resize(SHORTDESCRIPTION_MAXLEN);
	mShortDescription = _shortDescription;
}

void GameEntry::SetFullDescription(string _fullDescription)
{
	_fullDescription.resize(FULLDESCRIPTION_MAXLEN);
	mFullDescription = _fullDescription;
}


#pragma endregion



