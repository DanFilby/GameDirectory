#pragma once

#include <iostream>

#include "Common.h"
#include "Database.h"

enum EntryType : uint16_t { ET_Base = 0, ET_Game = 1, ET_Studio = 2 };
using ENTRYID = uint16_t;

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
		: Overall(_overall), Gameplay(_gameplay), Narrative(_Narrative), Replayability(_replayability) {
	}

	GameRatings()
		: Overall(), Gameplay(), Narrative(), Replayability() {
	}

	void const DisplayAllRatings() {
		std::cout << "Overall rating: " << StarRating(Overall) << "\n";
		std::cout << "Gameplay rating: " << StarRating(Gameplay) << "\n";
		std::cout << "Narrative rating: " << StarRating(Narrative) << "\n";
		std::cout << "Replayability rating: " << StarRating(Replayability) << "\n";
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

class GenreListDataBase : Database {

public:
	GenreListDataBase();

	string GetGenre(int hash);

	vector<string> mGenreList;

private:

	void DirectoriesInit();

	void LoadGenres();
	void UpdateGenreFile();

};

//list of genres stored locally, which can be added by users
//each object will be attached to a game entry and holds a list of genres
//the list will be weighted, meaning a total of 100 points to be split between genres
//hash genres which is how i will refernece them
struct GameGenres {





}; 