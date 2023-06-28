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

void GenreListDataBase::AddGenre(string genre)
{
	//limit the genre's length
	if (genre.size() > MAXLEN_GENRE) { genre.resize(MAXLEN_GENRE); }

	uint8_t genreKey = FindNewUniqueKey(genre, mGenreList);

	//check within maximum size
	if (mGenreList.size() < MAXCOUNT_GENRE && genreKey != 0) {
		//add genre, with its key, to the map
		mGenreList.emplace(std::make_pair(genreKey, genre));
	}
	else {
		std::cout << "Unable to add new genre\n";
	}
}

void GenreListDataBase::PrintGenreList()
{
	std::cout << "Genres: \n";
	for (const auto& genrePair : mGenreList) {
		std::cout<< (int)genrePair.first << " | " << genrePair.second << "\n";
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

void TagListDataBase::AddTag(string tag)
{
	//limit tag's length
	if (tag.size() > MAXLEN_TAGS) { tag.resize(MAXLEN_TAGS); }

	uint8_t tagKey = FindNewUniqueKey(tag, mTagList);


	//check within maximum size
	if (mTagList.size() < MAXCOUNT_TAGS) {
		//add the tag, with its key, to the map
		mTagList.emplace(std::make_pair(tagKey, tag));
	}
	else {
		std::cout << "Unable to add new tag\n";
	}
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
