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

	ENTRYID Id() { return mId; }
	string Name() { return mName; }
	uint16_t Year() { return mYear; }
	virtual const EntryType Type() { return mType; }

	/// <summary>
	/// returns a struct summarizing the entry: id, name and type
	/// </summary>
	virtual EntryInfo_Short GetSummary();
	/// <summary>
	/// returns a pointer to the summary in raw data format: byte array 
	/// </summary>
	virtual unique_ptr<char[]> GetRawData_Short();


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

private: const uint8_t BYTESIZE = 0;

public:
	GameEntry();
	GameEntry(char* rawData);

	~GameEntry();

	const EntryType Type() { return ET_Game; };

	EntryInfo_Short GetSummary();
	unique_ptr<char[]> GetRawData_Short();

public:
	//short description of the game max 64 chars
	string mShortDescription;

	//full description of the game maz 512 chars
	string mFullDescription;

	//set of ratings about various aspects of the game
	GameRatings mRatings;

	//list of studios which created this game
	vector<ENTRYID> mStudioIds;
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



