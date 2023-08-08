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

shared_ptr<char[]> Entry::GetRawData_Short()
{
	return GetSummary().ToBinary();
}

shared_ptr<char[]> Entry::GetBinaryData()
{
	return GetSummary().ToBinary();
}

EntryInfo_Short Entry::GetSummary() const
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, mType, mYear, mName };

	return info;
}

void Entry::PrintInfo()
{
}

bool Entry::IsEntryDataValid()
{
	if (!IsValid_Name(mName)) {
		std::cout << "Entry field invalid: Name\n"; return false;
	}
	if (!IsValid_Year(mYear)) {
		std::cout << "Entry field invalid: Year\n"; return false;
	}
	return true;
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
	:mGenres(), mTags(), mRatings(), mFinances(),
	mStudios(Relation_toStudios), mProducers(Relation_toProducers)
{
	mId = 0;
	mType = ET_Game;
	mYear = 0;
	mName = "";
}

GameEntry::GameEntry(ENTRYID _id, uint16_t _year, string _name)
	:GameEntry()
{
	mId = _id;
	mType = ET_Game;
	mYear = _year;
	mName = _name;
}

GameEntry::GameEntry(EntryInfo_Short _summary, shared_ptr<char[]> binaryData, shared_ptr<GenreListDataBase> _genreDatabase,
	shared_ptr<TagListDataBase> _tagDatabase): mFinances()
{
	mName = _summary.name; mYear = _summary.year; mId = _summary.id;

	uint16_t dataIndex = 0;

	mShortDescription.assign(&binaryData[dataIndex], &binaryData.get()[dataIndex + SHORTDESCRIPTION_MAXLEN]);
	dataIndex += SHORTDESCRIPTION_MAXLEN;

	mFullDescription.assign(&binaryData[dataIndex], &binaryData.get()[dataIndex + FULLDESCRIPTION_MAXLEN]);
	dataIndex += FULLDESCRIPTION_MAXLEN;

	mGenres = GameGenres(_genreDatabase, &binaryData[dataIndex]);
	dataIndex += GameGenres::BYTESIZE;

	mTags = GameTags(_tagDatabase, &binaryData[dataIndex]);
	dataIndex += GameTags::BYTESIZE;

	char* ratingsData = new char[GameRatings::BYTESIZE];
	memcpy(ratingsData, &binaryData[dataIndex], GameRatings::BYTESIZE);
	mRatings = GameRatings(ratingsData);
	dataIndex += GameRatings::BYTESIZE;

	char* financeData = new char[GameFinances::BYTESIZE];
	memcpy(financeData, &binaryData[dataIndex], GameFinances::BYTESIZE);
	mFinances = GameFinances(financeData);

	//TODO: look at stream buffers
}

GameEntry::~GameEntry()
{
}

EntryInfo_Short GameEntry::GetSummary() const
{
	//pass game's id, name and the game type into the struct
	EntryInfo_Short info{ mId, ET_Game, mYear, mName };
	return info;
}

shared_ptr<char[]> GameEntry::GetBinaryData()
{
	if (!IsEntryDataValid()) { std::cout << "Entry data invalid\n"; throw 01; }

	shared_ptr<char[]> data = shared_ptr<char[]>(new char[DATA_BYTESIZE]);
	uint16_t dataIndex = 0;

	memcpy(&data[dataIndex], &mShortDescription[0], mShortDescription.length());
	dataIndex += SHORTDESCRIPTION_MAXLEN;

	memcpy(&data[dataIndex], &mFullDescription[0], mFullDescription.length());
	dataIndex += FULLDESCRIPTION_MAXLEN;

	memcpy(&data[dataIndex], mGenres.ToBinary().get(), GameGenres::BYTESIZE);
	dataIndex += GameGenres::BYTESIZE;

	memcpy(&data[dataIndex], mTags.ToBinary().get(), GameTags::BYTESIZE);
	dataIndex += GameTags::BYTESIZE;

	memcpy(&data[dataIndex], mRatings.ToBinary().get(), GameRatings::BYTESIZE);
	dataIndex += GameRatings::BYTESIZE;

	memcpy(&data[dataIndex], mFinances.ToBinary().get(), GameFinances::BYTESIZE);
	dataIndex += GameFinances::BYTESIZE;

	return data;
}


void GameEntry::PrintInfo()
{
	std::cout << "\nGame entry: " << mName << "\n";
	std::cout << " Id: " << mId << " Year: " << mYear << "\n";
	std::cout << "- " << mShortDescription << " -\n\n";
	std::cout << "Tags: | " << mTags.GetTagsOneLine() << "\n";
	std::cout << "Genres: | " << mGenres.GetGenresOneLine() << "\n\n";
	std::cout << mFullDescription << "\n\n";
	mStudios.PrintRelations();
	mProducers.PrintRelations();
	mRatings.PrintRatings(); std::cout << "\n";
	mFinances.PrintFinances();
	std::cout << "\n";
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

bool GameEntry::IsEntryDataValid()
{
	if (!Entry::IsEntryDataValid()) { return false; }

	if (mShortDescription.length() != SHORTDESCRIPTION_MAXLEN) {
		std::cout << "Entry field invalid: Short description\n"; return false;
	}
	if (mFullDescription.length() != FULLDESCRIPTION_MAXLEN) {
		std::cout << "Entry field invalid: Full description\n"; return false;
	}

	return true;
}


#pragma endregion



