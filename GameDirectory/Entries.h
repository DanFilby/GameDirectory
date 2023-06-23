#pragma once

#include "Common.h"

using ENTRYID = uint16_t;

//will have to include how each entry links to others so can easily search for related, maybe list of tags/ genre

enum EntryType : uint16_t{ T_Base = 0, T_Game = 1, T_Studio = 2};
struct EntryInfo_Short;

class Entry {

public: 
	Entry();
	virtual ~Entry();

	ENTRYID Id();
	string Name();
	virtual const EntryType Type() { return T_Base; };

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
	string mName;
};

class GameEntry : public Entry{

private: const uint8_t BYTESIZE = 0;

public:
	GameEntry();
	GameEntry(char* rawData);

	~GameEntry();

	const EntryType Type() { return T_Game; };

	EntryInfo_Short GetInfo_Short();
	unique_ptr<char[]> GetRawData_Short();

	int Rating();

private:

	//list of studios which created this game
	vector<ENTRYID> mStudioIds;
	
	uint8_t mOverallRating;

};

class StudioEntry : public Entry {

private: const uint8_t BYTESIZE = 0;

public:
	StudioEntry();
	StudioEntry(char* rawData);	

	~StudioEntry();

	const EntryType Type() { return T_Studio; };

	EntryInfo_Short GetInfo_Short();
	unique_ptr<char[]> GetRawData_Short();

};

//Contains summurised info for each entry, limited to 32 bytes
struct EntryInfo_Short {

	EntryInfo_Short(ENTRYID _id, EntryType _type, string _name) :
		id(_id), type(_type)
	{
		//set-up name, restricting to max char count and filling with blank if less than
		for (size_t i = 0; i < 27; i++)
		{
			if (_name.length() > i) { name[i] = _name[i]; }
			else { name[i] = '\0'; }
		}
		name[27] = '\0';
	}

	//convert struct from binary storage
	EntryInfo_Short(char * binaryData):
		id(), type(), name()
	{
		if (binaryData != nullptr) {

			ENTRYID* _id = (ENTRYID*)&binaryData[0];
			EntryType* _type = (EntryType*)&binaryData[2];

			id = *_id;
			type = *_type;

			for (size_t i = 0; i <= 27; i++) { name[i] = binaryData[i + 4]; }
		}
	}

	//serialize struct to binary
	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[32]);

		//TODO: find out better way to do this, sstream?
		char* idPtr = (char*)&id;
		char* typePtr = (char*)&type;

		binaryData[0] = idPtr[0]; binaryData[1] = idPtr[1];
		binaryData[2] = typePtr[0]; binaryData[3] = typePtr[1];

		for (size_t i = 0; i <= 27; i++) { binaryData[i + 4] = name[i]; }

		return binaryData;
	}

	ENTRYID id;		//Entries' id (2 bytes)
	EntryType type;	//Entries' type (2 bytes)
	char name[28];	//Entries' name (28 bytes)

	static const int BYTESIZE = 32;
};