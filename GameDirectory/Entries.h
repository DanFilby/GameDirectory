#pragma once

#include "Common.h"

using ENTRYID = uint16_t;

//will have to include how each entry links to others so can easily search for related, maybe list of tags/ genre

enum EntryType : uint16_t{ ET_Base = 0, ET_Game = 1, ET_Studio = 2};
struct EntryInfo_Short;
struct GameRatings;

//Base Entry class containing interface for entries and all common member data: ID, Year, Name
class Entry {

public: 
	Entry();
	virtual ~Entry();

	ENTRYID Id();
	string Name();
	virtual const EntryType Type() { return ET_Base; };

	/// <summary>
	/// returns a struct summarizing the entry: id, name and type
	/// </summary>
	virtual EntryInfo_Short GetInfo_Short();
	/// <summary>
	/// returns a pointer to the summary in raw data format: byte array 
	/// </summary>
	virtual unique_ptr<char[]> GetRawData_Short();


protected:
	ENTRYID mId;
	uint16_t mYear;
	string mName;
};


//Video game entry: Will contain cover picture and ratings and descriptions
class GameEntry : public Entry {

private: const uint8_t BYTESIZE = 0;

public:
	GameEntry();
	GameEntry(char* rawData);

	~GameEntry();

	const EntryType Type() { return ET_Game; };

	EntryInfo_Short GetInfo_Short();
	unique_ptr<char[]> GetRawData_Short();

	int Rating();

private:

	//list of studios which created this game
	vector<ENTRYID> mStudioIds;

	uint8_t mOverallRating;

};

//Studio entry: an entry reperenting a video game studio 
class StudioEntry : public Entry {

private: const uint8_t BYTESIZE = 0;

public:
	StudioEntry();
	StudioEntry(char* rawData);

	~StudioEntry();

	const EntryType Type() { return ET_Studio; };

	EntryInfo_Short GetInfo_Short();
	unique_ptr<char[]> GetRawData_Short();

};


//Contains summurised info for each entry, limited to 32 bytes
struct EntryInfo_Short {
	static const uint8_t BYTESIZE = 32;
	static const uint8_t NAMELENGTH = 26;

	ENTRYID id;		//Entry's id (2 bytes)
	EntryType type;	//Entry's type (2 bytes)
	uint16_t year;	//Entry's Year (2 bytes)
	char name[NAMELENGTH];	//Entry's name (26 bytes)

	EntryInfo_Short(ENTRYID _id, EntryType _type, uint16_t _year, string _name) :
		id(_id), type(_type), year(_year)
	{
		//set-up name, restricting to max char count and filling with blank if less than
		for (size_t i = 0; i < NAMELENGTH - 1; i++)
		{
			if (_name.length() > i) { name[i] = _name[i]; }
			else { name[i] = '\0'; }
		}
		name[NAMELENGTH - 1] = '\0';
	}

	//convert struct from binary storage
	EntryInfo_Short(char* binaryData) :
		id(), type(), name(), year()
	{
		if (binaryData != nullptr) {
			//set entry info from the binary data
			memcpy(&id, &binaryData[0], sizeof(ENTRYID));
			memcpy(&type, &binaryData[2], sizeof(EntryType));
			memcpy(&year, &binaryData[4], sizeof(uint16_t));
			memcpy(&name, &binaryData[6], NAMELENGTH);
		}
	}

	//serialize struct to char bytes
	unique_ptr<char[]> ToBinary() const {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[32]);

		//TODO: find out better way to do this, sstream?
		char* idPtr = (char*)&id;
		char* typePtr = (char*)&type;
		char* yearPtr = (char*)&year;

		binaryData[0] = idPtr[0]; binaryData[1] = idPtr[1];
		binaryData[2] = typePtr[0]; binaryData[3] = typePtr[1];
		binaryData[4] = yearPtr[0]; binaryData[5] = yearPtr[1];

		for (size_t i = 0; i <= NAMELENGTH - 1; i++) { binaryData[i + 6] = name[i]; }

		return binaryData;
	}
};

//set of ratings for games, each rating is an 8 bit uint, with range of 0-20 to get 0-10 stars with halfs eg 7.5/10
struct GameRatings {
	uint8_t Overall, Gameplay, Narrative, Replayability;

	GameRatings(uint8_t _overall, uint8_t _gameplay, uint8_t _Narrative, uint8_t _replayability) 
		: Overall(_overall), Gameplay(_gameplay), Narrative(_Narrative), Replayability(_replayability)	{

	}

	void const DisplayAllRatings() {
		cout << "Overall rating: " << StarRating(Overall) << "\n";
		cout << "Gameplay rating: " << StarRating(Gameplay) << "\n";
		cout << "Narrative rating: " << StarRating(Narrative) << "\n";
		cout << "Replayability rating: " << StarRating(Replayability) << "\n";
	}
	
	inline const string StarRating(uint8_t rating) {
		string result;

		//add stars
		for (size_t i = 0; i < rating / 2; i++) { result += "* "; }

		//fill empty stars
		for (size_t i = 0; i < (10 - rating / 2); i++) { result += "0 "; }

		return result;
	}
};

//builder for video game has a studio step so can setup multiple studios

class EntryBuilder {




};

