#pragma once

#include <iostream>
#include <sstream>

#include "Common.h"
#include "Database.h"


enum EntryType : uint16_t { ET_Base = 0, ET_Game = 1, ET_Studio = 2, ET_Producer = 3 };
using ENTRYID = uint16_t;

//Contains summurised info for each entry, limited to 32 bytes
struct EntryInfo_Short {
	static const uint8_t BYTESIZE = 32;
	static const uint8_t NAMELENGTH = 26;

	ENTRYID id;		//Entry's id (2 bytes)
	EntryType type;	//Entry's type (2 bytes)
	uint16_t year;	//Entry's Year (2 bytes)
	char name[NAMELENGTH];	//Entry's name (26 bytes)

	EntryInfo_Short(){}

	EntryInfo_Short(ENTRYID _id, EntryType _type, uint16_t _year, string _name) :
		id(_id), type(_type), year(_year) {
		SetName(_name);
	}

	EntryInfo_Short(EntryType _type, uint16_t _year, string _name)
		:type(_type), year(_year), id(0) {
		SetName(_name);
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

	//overload == for custom comparison
	bool operator==(const EntryInfo_Short& compareEntry) const{
		//equal if name and year match
		if (strcmp(this->name, compareEntry.name) == 0 && this->year == compareEntry.year) {
			return true;
		}
		return false;
	}

	inline void SetName(const string& _name) {
		//set-up name, restricting to max char count and filling with blank if less than
		for (size_t i = 0; i < NAMELENGTH - 1; i++)
		{
			if (_name.length() > i) { name[i] = _name[i]; }
			else { name[i] = '\0'; }
		}
		name[NAMELENGTH - 1] = '\0';
	}

};

enum EntryRelationsType : uint16_t { Relation_toStudios = 0, Relation_toProducers = 1, Relation_toGames = 2 };

//wrapper for a list of entry ids
//either store 64 ids, or use a header and variable amount of ids -> requires changes to reading entries
struct EntryRelations {

	EntryRelationsType relationType;
	vector<ENTRYID> relations;

	EntryRelations(EntryRelationsType _relationType) : relationType(_relationType), relations() {
	}

	EntryRelations(vector<ENTRYID> _relations, EntryRelationsType relationType)
		:relations(_relations), relationType(relationType) {
	}

	void AddRelation(ENTRYID id) {
		relations.push_back(id);

		//remove duplicates
		std::sort(relations.begin(), relations.end());
		relations.erase(std::unique(relations.begin(), relations.end()), relations.end());
	}

	void PrintRelations() {
		std::cout << RelationTypeToString(relationType) << " Relations: ";
		for (const ENTRYID& entryRel : relations) {
			std::cout << (int)entryRel << ", ";
		}
		std::cout << "\n\n";
	}

	static const uint16_t MaxRelationsCount(EntryRelationsType relationType) {
		static const int maxRelations[] = { 16, 16, 256 };
		return maxRelations[(int)relationType];
	}

	static const string RelationTypeToString(EntryRelationsType relationType) {
		static const string relationNames[] = { "Studios", "Producers", "Games" };
		return relationNames[(int)relationType];
	}
};

struct SimpleDate {
	static const uint8_t BYTESIZE = 4;

	uint16_t year;
	uint16_t month;

	SimpleDate() :year(0), month(0) {}
	SimpleDate(uint16_t _year):year(_year), month(0){}
	SimpleDate(uint16_t _year, uint16_t _month) :year(_year), month(_month) {}
	SimpleDate(char* binData) :year(0), month(0) {
		memcpy(&year, &binData[0], sizeof(uint16_t));
		memcpy(&month, &binData[sizeof(uint16_t)], sizeof(uint16_t));
	}

	string GetDateAsString() { 
		std::stringstream ss;
		ss << (int)month <<" / " << (int)year;
		return ss.str();
	}

	int GetAge() { 
		time_t timeNow = time(0); struct tm localTime;
		localtime_s(&localTime ,&timeNow);
		return (1900 + localTime.tm_year) - year + ((localTime.tm_mon + 1 < month) ? -1 : 0);
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[4]);
		memcpy(&binaryData[0], &year, sizeof(uint16_t));
		memcpy(&binaryData[sizeof(uint16_t)], &month, sizeof(uint16_t));
		return binaryData;
	}
};


struct Person {
	static const uint8_t NAME_MAXLEN = 32;
	static const uint8_t BYTESIZE = NAME_MAXLEN + SimpleDate::BYTESIZE;

	string FullName;
	SimpleDate DateBorn;

	Person() :FullName(), DateBorn() {}
	Person(string _FullName) :DateBorn(), FullName(_FullName) {
		FullName.resize(32);
	}
	Person(string _FullName, SimpleDate _DateBorn) :DateBorn(_DateBorn), FullName(_FullName) {
		FullName.resize(32);
	}
	Person(char* binData) :FullName(), DateBorn() {
		char* strData = new char[NAME_MAXLEN];
		memcpy(strData, &binData[0], NAME_MAXLEN);
		FullName = string(strData);

		DateBorn = SimpleDate(&binData[NAME_MAXLEN]);
	}

	void Print() {
		std::cout << FullName << " - Born: " << DateBorn.GetDateAsString() << " - Age: " << DateBorn.GetAge() << "\n\n";
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], &FullName[0], NAME_MAXLEN);
		memcpy(&binaryData[NAME_MAXLEN], &DateBorn.ToBinary()[0], SimpleDate::BYTESIZE);
		return binaryData;
	}

};


#pragma region GameEntryUtils

//set of ratings for games, each rating is an 8 bit uint, with range of 0-20 to get 0-10 stars with halfs eg 7.5/10
struct GameRatings {
	static const uint8_t BYTESIZE = 4;

	uint8_t Overall, Gameplay, Narrative, Replayability;

	/// <summary>
	/// Initalise game ratings usings 8 bytes each, ranging from 0-20, actual rating is halved to allow for 7.5 etc.
	/// </summary>
	GameRatings(uint8_t _overall, uint8_t _gameplay, uint8_t _Narrative, uint8_t _replayability)
		: Overall(_overall), Gameplay(_gameplay), Narrative(_Narrative), Replayability(_replayability) {
	}

	GameRatings(float _overall, float _gameplay, float _Narrative, float _replayability) {
		Overall = FormatRating(_overall);
		Gameplay = FormatRating(_gameplay);
		Narrative = FormatRating(_Narrative);
		Replayability = FormatRating(_replayability);
	}

	GameRatings(char* binaryData)
		:Overall(), Gameplay(), Narrative(), Replayability() {
		memcpy(&Overall, &binaryData[0], sizeof(uint8_t));
		memcpy(&Gameplay, &binaryData[1], sizeof(uint8_t));
		memcpy(&Narrative, &binaryData[2], sizeof(uint8_t));
		memcpy(&Replayability, &binaryData[3], sizeof(uint8_t));
	}

	GameRatings()
		: Overall(), Gameplay(), Narrative(), Replayability() {
	}

	void PrintRatings() const {
		std::cout << "Overall rating: " << FloatRating(Overall) << "/ 10  " << StarRating(Overall) << "\n";
		std::cout << "Gameplay rating: " << FloatRating(Gameplay) << "/ 10  " << StarRating(Gameplay) << "\n";
		std::cout << "Narrative rating: " << FloatRating(Narrative) << "/ 10  " << StarRating(Narrative) << "\n";
		std::cout << "Replayability rating: " << FloatRating(Replayability) << "/ 10  " << StarRating(Replayability) << "\n";
	}

	inline string StarRating(uint8_t rating) const {
		string result;

		//add stars
		for (size_t i = 0; i < rating / 2; i++) { result += "* "; }

		//fill empty stars
		for (size_t i = 0; i < (10 - rating / 2); i++) { result += "O "; }

		return result;
	}

	inline uint8_t FormatRating(float rating) {
		return uint8_t(std::clamp(rating, 0.0f, 10.0f) * 2);
	}

	inline float FloatRating(uint8_t rating) const {
		return rating / 2.0f;
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[4]);
		binaryData[0] = char(Overall); binaryData[1] = char(Gameplay);
		binaryData[2] = char(Narrative); binaryData[3] = char(Replayability);

		return binaryData;
	}

};

//list of genres stored locally, which can be added by users
//each object will be attached to a game entry and holds a list of genres
//TODO: list will be weighted, meaning a total of 100 points to be split between genres
struct GameGenres {
	static const uint8_t NUM_GENRES = 8;
	static const uint8_t BYTESIZE = NUM_GENRES;

	uint8_t genreIds[NUM_GENRES];

	shared_ptr<GenreListDataBase> genreDatabase;

	GameGenres() {
		//set up empty ids and genre database
		memset(&genreIds, 0, NUM_GENRES);
		genreDatabase = nullptr;
	}

	GameGenres(shared_ptr<GenreListDataBase> _genreDatabase) {
		//set up empty ids and genre database
		memset(&genreIds, 0, NUM_GENRES);
		genreDatabase = _genreDatabase;
	}

	GameGenres(shared_ptr<GenreListDataBase> _genreDatabase, char* binaryData) {
		//set up empty ids and genre database
		genreDatabase = _genreDatabase;
		memcpy(&genreIds[0], binaryData, BYTESIZE);
	}

	void AddGenre(uint8_t genreKey) {
		if (!genreDatabase || !genreDatabase->GenreExsists(genreKey)) { std::cout << "Unable to add genre\n"; return; }

		for (auto& curId : genreIds) {
			if (curId == 0) {
				//found empty genre slot, update with new genre and return
				curId = genreKey;
				return;
			}
		}
		//if ids list is full replace last genre
		genreIds[NUM_GENRES - 1] = genreKey;
	}

	void AddGenre(uint8_t genreKey, int index) {
		if (!genreDatabase || !genreDatabase->GenreExsists(genreKey)) { std::cout << "Unable to add genre\n"; return; }

		index = std::clamp(index, 0, NUM_GENRES - 1);
		genreIds[index] = genreKey;
	}

	vector<string> GetGenres() {
		if (!genreDatabase) { return vector<string>(); }

		vector<string> output;

		for (const uint8_t& genreKey : genreIds) {
			output.push_back(genreDatabase->GetGenre(genreKey));
		}

		return output;
	}

	void PrintGenres() {
		if (!genreDatabase) { return; }

		for (uint8_t genreKey : genreIds) {
			if (genreKey == 0) { break; }
			std::cout << "Genre: " << (int)genreKey << " | " << genreDatabase->GetGenre(genreKey) << "\n";
		}
		std::cout << "\n";
	}

	string GetGenresOneLine() {
		if (!genreDatabase) { return string(); }

		string output;
		for (uint8_t genreKey : genreIds) {
			if (genreKey == 0) { break; }
			output += genreDatabase->GetGenre(genreKey) + " | ";
		}
		return output;
	}

	unique_ptr<char[]> ToBinary() const {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[NUM_GENRES]);

		std::memcpy(&binaryData.get()[0], &genreIds[0], BYTESIZE);

		return binaryData;
	}
};

/// <summary>
/// list of tags atached to game entries, using an array of ids and the tag database
/// </summary>
struct GameTags {
	static const uint8_t NUM_TAGS = 16;
	static const uint8_t BYTESIZE = NUM_TAGS;

	uint8_t tagIds[NUM_TAGS];

	shared_ptr<TagListDataBase> tagDatabase;

	GameTags() {
		//set up empty ids and database
		memset(&tagIds, 0, NUM_TAGS);
		tagDatabase = nullptr;
	}

	GameTags(shared_ptr<TagListDataBase> _tagDatabase) {
		//set up empty ids and database
		memset(&tagIds, 0, NUM_TAGS);
		tagDatabase = _tagDatabase;
	}

	GameTags(shared_ptr<TagListDataBase> _tagDatabase, char* binaryData) {
		//set up empty ids and database
		tagDatabase = _tagDatabase;
		memcpy(&tagIds[0], binaryData, BYTESIZE);
	}

	void AddTag(uint8_t tagKey) {
		if (!tagDatabase || !tagDatabase->TagExsists(tagKey)) { std::cout << "Unable to add tag\n"; return; }

		for (auto& curId : tagIds) {
			if (curId == 0) {
				//found empty tag slot, update with new tag and return
				curId = tagKey;
				return;
			}
		}
		//if ids list is full replace last tag
		tagIds[NUM_TAGS - 1] = tagKey;
	}

	void AddTag(uint8_t tagKey, int index) {
		if (!tagDatabase || !tagDatabase->TagExsists(tagKey)) { std::cout << "Unable to add tag\n"; return; }

		index = std::clamp(index, 0, NUM_TAGS - 1);
		tagIds[index] = tagKey;
	}

	vector<string> GetTags() {
		if (!tagDatabase) { return vector<string>(); }

		vector<string> output;
		for (const uint8_t& tagKey : tagIds) {
			output.push_back(tagDatabase->GetTag(tagKey));
		}
		return output;
	}

	void PrintTags() {
		if (!tagDatabase) { return; }

		for (uint8_t tagKey : tagIds) {
			if (tagKey == 0) { break; }
			std::cout << "Tag: " << (int)tagKey << " | " << tagDatabase->GetTag(tagKey) << "\n";
		}
		std::cout << "\n";
	}

	string GetTagsOneLine() {
		if (!tagDatabase) { return string(); }

		string output;
		for (uint8_t tagKey : tagIds) {
			if (tagKey == 0) { break; }
			output += tagDatabase->GetTag(tagKey) + " | ";
		}
		return output;
	}

	unique_ptr<char[]> ToBinary() const {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[NUM_TAGS]);

		std::memcpy(&binaryData.get()[0], &tagIds[0], BYTESIZE);

		return binaryData;
	}
};

struct GameFinances {
	static const uint8_t BYTESIZE = sizeof(float) * 4;

	enum SalesFactor { Billions = 0, Millions = 1, HundredThousands = 2, Thousands = 3, none = 4 };

	float salesEU, salesNA, salesAsia, salesOther;
	float salesTotal;

	GameFinances()
		:salesEU(0), salesNA(0), salesAsia(0), salesOther(0), salesTotal(0) {}

	GameFinances(float _salesEU, float _salesNA, float _salesAsia, float _salesOther)
		:salesEU(_salesEU), salesNA(_salesNA), salesAsia(_salesAsia), salesOther(_salesOther) {
		salesTotal = salesEU + salesNA + salesAsia + salesOther;
	}

	GameFinances(char* binaryData) {
		memcpy(&salesEU, &binaryData[sizeof(float) * 0], sizeof(float));
		memcpy(&salesNA, &binaryData[sizeof(float) * 1], sizeof(float));
		memcpy(&salesAsia, &binaryData[sizeof(float) * 2], sizeof(float));
		memcpy(&salesOther, &binaryData[sizeof(float) * 3], sizeof(float));
		salesTotal = salesEU + salesNA + salesAsia + salesOther;
	}

	void PrintFinances() {
		float averageSales = salesTotal / 3.0f;
		float lowestSales = min(min(salesEU, salesNA), salesAsia);

		//use average if lowest sales are an anomaly
		if (averageSales - lowestSales > 1000) { lowestSales = averageSales; }

		SalesFactor highestFactor = FindHigestFactor(lowestSales);

		PrintFinances(highestFactor);
	}

	void PrintFinances(SalesFactor salesFactor) {
		std::cout << "Finances:\n";
		std::cout << "Europe Sales: " << SalesToString(salesNA, salesFactor) << "\n";
		std::cout << "North America Sales: " << SalesToString(salesEU, salesFactor) << "\n";
		std::cout << "Asia Sales: " << SalesToString(salesAsia, salesFactor) << "\n";
		std::cout << "Other Sales: " << SalesToString(salesOther, salesFactor) << "\n";
		std::cout << "Total Sales: " << SalesToString(salesTotal, salesFactor) << "\n\n";
	}

	inline string SalesToString(float sales, SalesFactor salesFactor) {
		float salesFactorised = sales / FactorToNum(salesFactor);

		std::stringstream salesString;
		salesString << std::fixed << std::setprecision(3) << salesFactorised;

		return salesString.str() + " " + FactorToString(salesFactor);
	}

	const SalesFactor FindHigestFactor(float val) {
		if (val > FactorToNum(Billions)) { return Billions; }
		else if (val > FactorToNum(Millions)) { return Millions; }
		else if (val > FactorToNum(HundredThousands)) { return HundredThousands; }
		else if (val > FactorToNum(Thousands)) { return Thousands; }
		else { return none; }
	}

	const inline string FactorToString(SalesFactor salesFactor) {
		static const string factorsNames[] = { "Billion", "Million", "Hundred Thousand", "Thousand", "" };
		return factorsNames[(int)salesFactor];
	}

	const inline float FactorToNum(SalesFactor salesFactor) {
		static const float factorsNum[] = { 1000000000.0f, 1000000.0f, 100000.0f, 1000.0f, 1.0f };
		return factorsNum[(int)salesFactor];
	}

	unique_ptr<char[]> ToBinary() const {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[sizeof(float) * 0], &salesEU, sizeof(float));
		memcpy(&binaryData[sizeof(float) * 1], &salesNA, sizeof(float));
		memcpy(&binaryData[sizeof(float) * 2], &salesAsia, sizeof(float));
		memcpy(&binaryData[sizeof(float) * 3], &salesOther, sizeof(float));
		return binaryData;
	}
};

#pragma endregion

#pragma region StuidoEntryUtils

struct StudioExecutives {
	static const uint8_t FOUNDERS_MAXCOUNT = 3;
	static const uint8_t BYTESIZE = Person::BYTESIZE * (1 + FOUNDERS_MAXCOUNT);

	Person CEO;
	Person Founders[FOUNDERS_MAXCOUNT];

	//TODO: add further executives/ directors

	StudioExecutives() :CEO(), Founders() {}
	StudioExecutives(Person _CEO) :CEO(_CEO), Founders() {}
	StudioExecutives(Person _CEO, vector<Person> _Founders) :CEO(_CEO){
		_Founders.resize(FOUNDERS_MAXCOUNT);

		int count = 0;
		for (Person founder : _Founders) { Founders[count++] = founder; }
	}
	StudioExecutives(char* binData){
		CEO = Person(binData);
		for (size_t i = 0; i < FOUNDERS_MAXCOUNT; i++) { Founders[i] = Person(&binData[Person::BYTESIZE * (i + 1)]); }
	}

	void PrintAllExecs() {
		std::cout << "CEO: " << CEO.FullName << "\n";
		for(const Person& founder : Founders){ std::cout << "Founder: " << founder.FullName << "\n"; }
		std::cout << "\n";
	}
	
	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], CEO.ToBinary().get(), Person::BYTESIZE);
		for (size_t i = 0; i < FOUNDERS_MAXCOUNT; i++) {
			memcpy(&binaryData[Person::BYTESIZE * (i + 1)], Founders[i].ToBinary().get(), Person::BYTESIZE);
		}

		return binaryData;
	}
};


#pragma endregion

