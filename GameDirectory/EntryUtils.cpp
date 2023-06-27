#include "EntryUtils.h"

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

void GenreListDataBase::LoadGenres()
{
	if (!FileReadCheck(DIR_PATH + GENRELIST_FNAME)) {
		std::cout << "Error - " << GENRELIST_FNAME << " file not valid\n";
		return;
	}



}

void GenreListDataBase::UpdateGenreListFile()
{



}
