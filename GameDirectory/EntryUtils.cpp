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

	ifstream genreListFile = ifstream(DIR_PATH + GENRELIST_FNAME, std::ios::in);

	mGenreList.clear();
	string genreBuf;

	if (genreListFile.is_open()) {
		while (genreListFile.good()) {

			//read genre and add to list
			genreListFile >> genreBuf;
			mGenreList.push_back(string(genreBuf));
		}
	}

	genreListFile.close();
}


void GenreListDataBase::UpdateGenreListFile()
{
	//open files output stream, deletes old files contents
	ofstream genreListFile = ofstream(DIR_PATH + GENRELIST_FNAME, std::ios::out, std::ios::trunc);

	//loop over genres and add each as its own line 
	for (const string& genre : mGenreList) {
		genreListFile << genre << "\n";
	}

	genreListFile.close();
}

//TODO: create duplicate check function, sort as well