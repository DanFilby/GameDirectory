#pragma once

#include <string>
using std::string;

using ENTRYID = uint16_t;

//will have to include how each entree links to others so can easily search for related, maybe list of tags/ genre

class Entry {

public: 
	Entry();
	virtual ~Entry();

	ENTRYID ID();
	string NAME();
		
	virtual char* ToFile();

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

	char* ToFile();

	int Rating();

};

class StudioEntry : public Entry {

private: const uint8_t BYTESIZE = 0;

public:
	StudioEntry();
	StudioEntry(char* rawData);

	~StudioEntry();

	char* ToRawData();

};