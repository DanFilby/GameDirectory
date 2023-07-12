#include "Database.h"
#include <sstream>

bool Database::IsDirVaild(string dirPath)
{
	return std::filesystem::is_directory(std::filesystem::path(dirPath));
}

void Database::SetupDir(string dirPath)
{
	//convert to windows string format
	std::wstring temp = std::wstring(dirPath.begin(), dirPath.end());
	LPCWSTR wideString = temp.c_str();

	//check directory exsits
	if (IsDirVaild(dirPath)) {
		std::cout << dirPath << " - Valid\n\n";
		return;
	}
	else {
		//create directory 
		std::filesystem::create_directory(path(dirPath));
		std::cout << "Created - " << dirPath << "\n\n";
	}

}

bool Database::FileReadCheck(string filePath)
{
	//check file exsists
	if (!std::filesystem::exists(path(filePath))) {

		std::cout << filePath << " - file doesn't exsist\n";

		//create and check file
		ofstream file = ofstream(filePath);
		if (file.good()) { std::cout << filePath << " - file created\n\n"; file.close(); }
		else{ std::cout << "Failed to create: " << filePath << "\n\n"; }

		return false;
	}
	//check the file isn't empty
	else if (std::filesystem::is_empty(path(filePath))) {
		std::cout << filePath << " - file is empty\n\n";
		return false;
	}

	//otherwise file is fine and not empty
	std::cout << filePath << " - valid\n\n";

	return true;
}

void Database::DirectoriesCheck()
{
	SetupDir(DIR_PATH);
}

vector<string> StringFileMan::ReadStringFile(string filePath)
{
	ifstream fileStream = ifstream(filePath, std::ios::in);

	vector<string> fileContents;
	string stringBuf;

	//loop over file and insert each line into the output list
	if (fileStream.is_open()) {
		while (fileStream.good()) {

			//read the line and add to output list
			std::getline(fileStream, stringBuf);

			//add if not empty
			if (!stringBuf.empty()) { fileContents.push_back(string(stringBuf)); }
		}
	}
	
	fileStream.close();
	return fileContents;
}

void StringFileMan::WriteStringFile(string filePath, vector<string> contents)
{
	//open files output stream, deletes old files contents
	ofstream fileStream = ofstream(filePath, std::ios::out, std::ios::trunc);
	if (!fileStream.good()) { std::cout << "Failed to write to file: " << filePath << "\n"; return; }

	//loop over given contents and add each string as its own line 
	for (const string& line : contents) {
		fileStream << line << "\n";
	}

	fileStream.close();
}

void StringFileMan::SortAndUniqueStrings(vector<string>& strings)
{
	//sort and remove any duplicate genres
	std::sort(strings.begin(), strings.end());
	strings.erase(unique(strings.begin(), strings.end()), strings.end());
}

map<uint8_t, string> StringFileMan::ReadStringIndexFile(string filePath)
{
	ifstream fileStream = ifstream(filePath, std::ios::in);

	map<uint8_t, string> fileContents;
	string lineBuf, wordBuf;

	//loop over file
	if (fileStream.is_open()) {
		while (fileStream.good()) {

			//get a line and create a string stream from it
			std::getline(fileStream, lineBuf);
			std::stringstream streamBuffer(lineBuf);

			//get the index before of the comma
			std::getline(streamBuffer, wordBuf, ',');
			uint8_t index = uint8_t(wordBuf[0]);

			//get the word after the comma
			std::getline(streamBuffer, wordBuf, ',');
			string word = string(wordBuf);

			//add into map if not empty
			if (!index == 0 || !word.empty()) {
				fileContents.emplace(std::make_pair(index, word));
			}		
		}
	}

	fileStream.close();
	return fileContents;
}

void StringFileMan::WriteStringIndexFile(string filePath, map<uint8_t, string> contents)
{
	//open files output stream, deletes old files contents
	ofstream fileStream = ofstream(filePath, std::ios::out, std::ios::trunc);
	if (!fileStream.good()) { std::cout << "Failed to write to file: " << filePath << "\n"; return; }

	//loop over genres and add each as its own line 
	for (const auto& contentPair : contents) {

		//write to file if not empty
		if (!contentPair.first == 0 || !contentPair.second.empty()) {
			fileStream << contentPair.first << ',' << contentPair.second << "\n";
		}
	}

	fileStream.close();
}

uint8_t StringFileMan::FindNewUniqueKey(string word, const map<uint8_t, string>& currentMap)
{
	//get an index based roughly on the first character, between 1 - 255
	uint8_t charIndex = tolower(word[0]) - 97;
	uint8_t wordKey = (charIndex * 9) + 1;

	//loop until a unique key is found, by iterating the key if not unique
	bool foundUnique = false; int failsafe = 256;
	while (!foundUnique && failsafe > 0) {

		//check if key is unique
		if (!currentMap.count(wordKey) && wordKey != 0) {
			foundUnique = true;
		}
		//otherwise key already exsists, add one
		else {
			wordKey++;
			failsafe--;
		}
	}

	//return key if found a new unique one, otherwise return 0
	if (foundUnique) { return wordKey; }
	else { std::cout << "Unable to find unique genre key\n"; return 0; }
}

uint8_t StringFileMan::GetKey(const map<uint8_t, string>& currentMap, string value)
{
	uint8_t keyFound = 0;
	for (const auto& stringIndexPair : currentMap) {
		if (stringIndexPair.second == value) {
			keyFound = stringIndexPair.first;
		}
	}
	return keyFound;
}

vector<string> StringFileMan::GetValues(const map<uint8_t, string>& currentMap)
{
	vector<string> values;
	std::transform(currentMap.begin(), currentMap.end(), std::back_inserter(values), [](const std::pair<uint8_t, string>& val) {return val.second; });
	return values;
}

//abstract function
uint8_t StringFileMan::GetKey(string value)
{
	return 0;
}

GenreListDataBase::GenreListDataBase()
{
	DirectoriesCheck();

	LoadGenres();
}


void GenreListDataBase::DirectoriesCheck()
{
	//check data directory is valid	
	assert(IsDirVaild(DIR_PATH));
}

string GenreListDataBase::GetGenre(uint8_t key)
{
	if (GenreExsists(key)) {
		return string(mGenreList[key]);
	}
	return string();
}

uint8_t GenreListDataBase::GetKey(string genre)
{
	return StringFileMan::GetKey(mGenreList, genre);
}

bool GenreListDataBase::GenreExsists(uint8_t genreKey)
{
	return mGenreList.find(genreKey) != mGenreList.end();
}

bool GenreListDataBase::GenreExsists(string genre)
{
	return GetKey(genre) != 0;
}


void GenreListDataBase::AddGenre(string genre)
{
	if (GenreExsists(genre)) { std::cout << "Duplicate genre\n"; return; }

	//limit the genre's length
	if (genre.size() > MAXLEN_GENRENAME) { genre.resize(MAXLEN_GENRENAME); }

	uint8_t genreKey = FindNewUniqueKey(genre, mGenreList);

	//check within maximum size
	if (mGenreList.size() < MAXCOUNT_GENRE && genreKey != 0) {
		mGenreList.emplace(std::make_pair(genreKey, genre));
		UpdateGenreListFile();
	}
	else {
		std::cout << "Unable to add new genre\n";
	}
}

void GenreListDataBase::RemoveGenre(string genre)
{
	if (GenreExsists(genre)) {
		mGenreList.erase(GetKey(genre));
		UpdateGenreListFile();
	}
}

vector<string> GenreListDataBase::GetAllGenres()
{
	return GetValues(mGenreList);
}

void GenreListDataBase::PrintGenreList()
{
	std::cout << "Genres: \n";
	for (const auto& genrePair : mGenreList) {
		std::cout << (int)genrePair.first << " | " << genrePair.second << "\n";
	}
	std::cout << "\n";
}

//Loads genres from file
void GenreListDataBase::LoadGenres()
{
	//check file's valid
	if (!FileReadCheck(DIR_PATH + GENRELIST_FNAME)) {
		std::cout << "Error - " << GENRELIST_FNAME << " file not valid\n";
		return;
	}

	mGenreList = ReadStringIndexFile(DIR_PATH + GENRELIST_FNAME);
}


void GenreListDataBase::UpdateGenreListFile()
{
	WriteStringIndexFile(DIR_PATH + GENRELIST_FNAME, mGenreList);
}

TagListDataBase::TagListDataBase()
{
	DirectoriesCheck();

	LoadTags();
}

void TagListDataBase::DirectoriesCheck()
{
	//check data directory is valid	
	assert(IsDirVaild(DIR_PATH));
}

string TagListDataBase::GetTag(uint8_t key)
{
	return string(mTagList[key]);
}

uint8_t TagListDataBase::GetKey(string tag)
{
	return StringFileMan::GetKey(mTagList, tag);
}

bool TagListDataBase::TagExsists(uint8_t tagKey)
{
	return mTagList.find(tagKey) != mTagList.end();
}

bool TagListDataBase::TagExsists(string tag)
{
	return GetKey(tag) != 0;
}

void TagListDataBase::AddTag(string tag)
{
	if (TagExsists(tag)) { std::cout << "Duplicate tag\n"; return; }

	//limit tag's length
	if (tag.size() > MAXLEN_TAGS) { tag.resize(MAXLEN_TAGS); }

	uint8_t tagKey = FindNewUniqueKey(tag, mTagList);


	//check within maximum size
	if (mTagList.size() < MAXCOUNT_TAGS) {
		mTagList.emplace(std::make_pair(tagKey, tag));
		UpdateTagListFile();
	}
	else {
		std::cout << "Unable to add new tag\n";
	}
}

void TagListDataBase::RemoveTag(string tag)
{
	if (TagExsists(tag)) {
		mTagList.erase(GetKey(tag));
		UpdateTagListFile();
	}
}

vector<string> TagListDataBase::GetAllTags()
{
	return GetValues(mTagList);
}

void TagListDataBase::PrintTagList()
{
	std::cout << "Tags: \n";
	for (const auto& tagPair : mTagList) {
		std::cout << (int)tagPair.first << " | " << tagPair.second << "\n";
	}
	std::cout << "\n";
}

void TagListDataBase::LoadTags()
{
	//check file's valid
	if (!FileReadCheck(DIR_PATH + TAGLIST_FNAME)) {
		std::cout << "Error - " << TAGLIST_FNAME << " file not valid\n";
		return;
	}

	mTagList = ReadStringIndexFile(DIR_PATH + TAGLIST_FNAME);
}

void TagListDataBase::UpdateTagListFile()
{
	WriteStringIndexFile(DIR_PATH + TAGLIST_FNAME, mTagList);
}