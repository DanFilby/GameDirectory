#pragma once

#include <iostream>

#include "Common.h"
#include "EntryUtils.h"


//Base Entry class containing interface for entries and all common member data: ID, Year, Name
class Entry {
	friend class EntryBuilder;
	friend class EntryDatabase;

public:
	static const uint8_t MINLEN_ENTRYNAME = 2;
	static const uint8_t MAXLEN_ENTRYNAME = 32;

public:
	Entry();
	Entry(ENTRYID _id, EntryType _type, uint16_t _year, string _name);
	virtual ~Entry();

	/// <summary>
	/// comparison of: name, year, type, or id 
	/// </summary>
	bool operator==(const Entry& compareEntry);

	/// <summary>
	/// returns a struct summarizing the entry: id, name and type
	/// </summary>
	virtual EntryInfo_Short GetSummary();
	/// <summary>
	/// returns a summary of the entry in byte format 
	/// </summary>
	virtual unique_ptr<char[]> GetRawData_Short();

	ENTRYID const Id() { return mId; }
	string const Name() { return mName; }
	uint16_t const Year() { return mYear; }
	virtual const EntryType Type() { return mType; }

protected:
	ENTRYID mId;
	EntryType mType;
	uint16_t mYear;
	string mName;

	virtual bool IsValid_Name(const string& _name);
	virtual bool IsValid_Year(const uint16_t _year);
};

#pragma region GameEntry

//Video game entry: Will contain cover picture and ratings and descriptions
class GameEntry : public Entry {

public:
	static const uint8_t BYTESIZE = 0;
	static const uint8_t FULLDESCRIPTION_MAXLEN = 255;
	static const uint8_t SHORTDESCRIPTION_MAXLEN = 63;


public:
	GameEntry();
	GameEntry(ENTRYID _id, uint16_t _year, string _name);
	GameEntry(char* rawData);

	~GameEntry();

	EntryInfo_Short GetSummary();
	unique_ptr<char[]> GetRawData_Short();

	void PrintInfo();

	EntryType const Type() { return ET_Game; };

	//getters and setters for descriptions as they're length restricted
	string const ShortDescription() { return mShortDescription; }
	void SetShortDescription(string _shortDescription);
	string const FullDescription() { return mFullDescription; }
	void SetFullDescription(string _fullDescription);

public:
	//list of studios which created this game
	vector<ENTRYID> mStudioIds;

	//genres and tags related to the game entry
	GameGenres mGenres;
	GameTags mTags;

	//set of ratings about various aspects of the game
	GameRatings mRatings;

private:
	//short description of the game, max length specified
	string mShortDescription;

	//full description of the game, max length specified
	string mFullDescription;

};


#pragma endregion

//Studio entry: an entry reperenting a video game studio 
class StudioEntry : public Entry {

private: const uint8_t BYTESIZE = 0;

public:
	StudioEntry();
	StudioEntry(char* rawData);

	~StudioEntry();

	const EntryType Type() { return ET_Studio; };

	EntryInfo_Short GetSummary();
	unique_ptr<char[]> GetRawData_Short();

};



