#pragma once

#include "Common.h"
#include "EntryUtils.h"


//Base Entry class containing interface for entries and all common member data: ID, Year, Name
class Entry {
	//TODO: platforms, publisher

public:
	Entry();
	Entry(EntryInfo_Short _entrySummary);
	Entry(ENTRYID _id, EntryType _type, uint16_t _year, string _name);
	virtual ~Entry();

	/// <summary>
	/// comparison of: (name, year, type) or id 
	/// </summary>
	bool operator==(const Entry& compareEntry) const;

	virtual uint16_t GetDataByteSize();
	virtual shared_ptr<char[]> GetBinaryData();

	virtual vector<EntryRelations*> GetRelations();


	virtual void SetBaseInfo(EntryInfo_Short info);
	virtual EntryInfo_Short GetSummary() const;

	virtual void PrintInfo();

	ENTRYID  Id() const { return mId; }
	string Name() const { return mName; }
	uint16_t Year() const { return mYear; }
	virtual EntryType Type() const { return mType; }

protected:
	ENTRYID mId;
	EntryType mType;
	uint16_t mYear;
	string mName;

	virtual bool IsEntryDataValid();

	virtual bool IsValid_Name(const string& _name);
	virtual bool IsValid_Year(const uint16_t _year);

public:
	static const uint8_t MINLEN_ENTRYNAME = 2;
	static const uint8_t MAXLEN_ENTRYNAME = 26;

	friend class EntryBuilder;
	friend class EntryDatabase;
};


#pragma region GameEntry

//Video game entry: Will contain cover picture and ratings and descriptions
class GameEntry : public Entry {
public:
	GameEntry();
	GameEntry(ENTRYID _id, uint16_t _year, string _name);
	GameEntry(EntryInfo_Short _summary, shared_ptr<char[]> binaryData, shared_ptr<GenreListDataBase> _genreDatabase, shared_ptr<TagListDataBase> _tagDatabase);
	~GameEntry();

	uint16_t GetDataByteSize() override;
	shared_ptr<char[]> GetBinaryData() override;

	vector<EntryRelations*> GetRelations() override;

	EntryInfo_Short GetSummary() const override;
	void PrintInfo() override;

	//getters and setters for descriptions as they're length restricted
	string ShortDescription() const { return mShortDescription; }
	string FullDescription() const { return mFullDescription; }
	void SetShortDescription(string _shortDescription);
	void SetFullDescription(string _fullDescription);

	EntryType Type() const { return ET_Game; };

private:
	bool IsEntryDataValid();

public:
	GameGenres mGenres;
	GameTags mTags;

	//set of ratings about various aspects of the game
	GameRatings mRatings;

	GameFinances mFinances;

	EntryRelations mStudios{ EntryRelationsType::Relation_toStudios };
	EntryRelations mProducers{ EntryRelationsType::Relation_toProducers };

private:
	string mShortDescription;
	string mFullDescription;

public:
	static const uint8_t SHORTDESCRIPTION_MAXLEN = 63;
	static const uint8_t FULLDESCRIPTION_MAXLEN = 255;

	static const uint16_t DATA_BYTESIZE = SHORTDESCRIPTION_MAXLEN + FULLDESCRIPTION_MAXLEN + GameGenres::BYTESIZE
		+ GameTags::BYTESIZE + GameRatings::BYTESIZE + GameFinances::BYTESIZE;

	friend class GameEntryBuilder;
};

#pragma endregion

#pragma region StudioEntry

//Studio entry: an entry reperenting a video game studio 
class StudioEntry : public Entry {

public:
	StudioEntry();
	StudioEntry(string _name, uint16_t _year, ENTRYID _id);
	StudioEntry(EntryInfo_Short _summary, shared_ptr<char[]> binaryData);

	~StudioEntry();

	uint16_t GetDataByteSize() override;
	shared_ptr<char[]> GetBinaryData() override;

	vector<EntryRelations*> GetRelations() override;

	EntryInfo_Short GetSummary() const override;
	void PrintInfo() override;

	EntryType Type() const { return ET_Studio; };
	
	string GetDescription() { return mDescription; }
	void SetDescription(string _stuioDesctription);

private:
	virtual bool IsEntryDataValid();

public:
	EntryRelations mGamesDeveloped{ Relation_toGames };

	StudioExecutives mExecutives;

	//finances

	//employees, count, num programers & leaders etc.

	uint16_t mNumGamesReleased{0};

private:
	string mDescription;

public:
	static const uint16_t DESCRIPTION_MAXLEN = 512;
	static const uint16_t DATA_BYTESIZE = DESCRIPTION_MAXLEN + sizeof(mNumGamesReleased) + StudioExecutives::BYTESIZE;
};


#pragma endregion






