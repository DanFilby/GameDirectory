#include "EntryUtils.h"

GenreListDataBase::GenreListDataBase()
{
	DirectoriesCheck();

	LoadGenres();
}

void GenreListDataBase::AddGenre(string genre)
{
	//limit the genre's length
	if (genre.size() > MAXLEN_GENRE) { genre.resize(MAXLEN_GENRE); }

	//check within maximum size
	if (mGenreList.size() < MAXCOUNT_GENRE) {
		mGenreList.push_back(genre);
		SortAndUniqueStrings(mGenreList);
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
	SortAndUniqueStrings(mGenreList);
	WriteStringFile(DIR_PATH + GENRELIST_FNAME, mGenreList);
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

void TagListDataBase::AddTag(string tag)
{
	//limit tag's length
	if (tag.size() > MAXLEN_TAGS) { tag.resize(MAXLEN_TAGS); }

	//check within maximum size
	if (mTagList.size() < MAXCOUNT_TAGS) {
		mTagList.push_back(tag);
		SortAndUniqueStrings(mTagList);
	}
	else {
		std::cout << "Max genre count reached";
	}
}

void TagListDataBase::PrintTagList()
{
	std::cout << "Tags: \n";
	for (const string& tag : mTagList) {
		std::cout << tag << "\n";
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

	mTagList = LoadStringFile(DIR_PATH + TAGLIST_FNAME);
}

void TagListDataBase::UpdateTagListFile()
{
	SortAndUniqueStrings(mTagList);
	WriteStringFile(DIR_PATH + TAGLIST_FNAME, mTagList);
}
