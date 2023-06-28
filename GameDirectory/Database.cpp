#include "Database.h"

#include <sstream>

void DatabaseManager::AppInit()
{
	SetupDir(DIR_PATH);
}

void DatabaseManager::Close()
{
}

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


