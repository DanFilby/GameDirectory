#include "EntryUtils.h"

GenreListDataBase::GenreListDataBase()
{
	DirectoriesCheck();

	LoadGenres();
}

void GenreListDataBase::AddGenre(string genre)
{
	//check within maximum size
	if (mGenreList.size() < MAXGENRECOUNT) {
		mGenreList.push_back(genre);
		SortGenres();
	}
	else {
		std::cout << "Max genre count reached";
	}

}


void GenreListDataBase::DirectoriesCheck()
{
	//check data directory is valid	
	assert(IsDirVaild(DIR_PATH));
}

void GenreListDataBase::SortGenres()
{
	//sort and remove any duplicate genres
	std::sort(mGenreList.begin(), mGenreList.end());
	mGenreList.erase(unique(mGenreList.begin(), mGenreList.end()), mGenreList.end());
}

void GenreListDataBase::PrintGenreList()
{
	std::cout << "Genres: \n";
	for (const string& genre : mGenreList) {
		std::cout << genre << "\n";
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

	mGenreList = LoadStringFile(DIR_PATH + GENRELIST_FNAME);
}


void GenreListDataBase::UpdateGenreListFile()
{
	WriteStringFile(DIR_PATH + GENRELIST_FNAME, mGenreList);
}