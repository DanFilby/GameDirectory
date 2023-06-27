#include "Database.h"

void Database::Init()
{
	SetupDir(DIR_PATH);
}

void Database::Close()
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

		std::cout << filePath << " - file don't exsist\n";

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
	std::cout << filePath << " - file is valid\n\n";

	return true;
}
