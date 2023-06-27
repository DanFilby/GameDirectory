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
		std::cout << dirPath << " - Valid\n";
		return;
	}
	else {
		//create directory 
		std::filesystem::create_directory(std::filesystem::path(dirPath));
		std::cout << "Created - " << dirPath << "\n";
	}

}
