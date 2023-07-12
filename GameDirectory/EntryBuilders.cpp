#include "EntryBuilders.h"

EntryBuilder::EntryBuilder(shared_ptr<EntryDatabase> _entryDatabase)
	:mEntryDatabase(_entryDatabase) {
	ClearBuild();
	srand((unsigned int)time(NULL));
}

void EntryBuilder::ClearBuild()
{
	if (mCurrentEntry) { delete(mCurrentEntry); }
	mCurrentEntry = new Entry();
}

void EntryBuilder::EditEntry(Entry& _entry)
{
	ClearBuild();
	mCurrentEntry = new Entry(_entry);
}

void EntryBuilder::EditEntry(ENTRYID _entryId)
{
	if (mEntryDatabase->EntryExsists(_entryId)) { std::cout << "Entry doesn't exsist to edit\n"; return; }
	return;
	//TODO: when implented entries to database rather than the summary
}

bool EntryBuilder::BuildEntry(shared_ptr<Entry>& entry)
{
	//check all required entry fields are filled
	if (!RequiredFieldsCheck()) {
		std::cout << "Required entry are fields incomplete\n";
		return false;
	}

	//create a copy of the temp entry
	Entry outputEntry = Entry(*mCurrentEntry);
	
	//set id to 0, as it's not being added to the database right away 
	outputEntry.mId = 0;

	//set the returned out entry 
	entry = make_shared<Entry>(outputEntry);

	return true;
}

bool EntryBuilder::BuildAndSaveEntry(shared_ptr<Entry>& entry)
{
	//build the entry normally, check if valid
	if (BuildEntry(entry)) { return false; }

	//get and set a unique id for this entry
	bool databaseCheck = mEntryDatabase->SetUniqueId(*entry);

	//return if failed database checks
	if (!databaseCheck) { std::cout << "Entry failed database checks\n"; return false; }

	//add entry to database, and save to file
	mEntryDatabase->AddEntry(*entry);
	mEntryDatabase->UpdateEntriesFile();

	return true;
}

void EntryBuilder::SetInfo(EntryInfo_Short info)
{
	SetNameYear(info.name, info.year);
	mCurrentEntry->mId = info.id;
}

void EntryBuilder::SetNameYear(string _name, uint16_t _year)
{
	SetName(_name);
	SetYear(_year);
}

void EntryBuilder::SetName(string _name)
{
	//check valid name
	if (mCurrentEntry->IsValid_Name(_name)) {
		mCurrentEntry->mName = _name;
	}
	else { std::cout << "Invalid name given to entry\n"; }
}

void EntryBuilder::SetYear(uint16_t _year)
{
	//check valid year
	if (mCurrentEntry->IsValid_Year(_year)) {
		mCurrentEntry->mYear = _year;
	}
	else { std::cout << "Invalid year given to entry\n"; }
}

bool EntryBuilder::RequiredFieldsCheck()
{
	//check if year and name have been assigned
	if (mCurrentEntry->mYear == 0) {
		std::cout << "Entry missing required field: " << "Year\n";
		return false;
	}
	if (mCurrentEntry->mName.empty()) {
		std::cout << "Entry missing required field: " << "Name\n";
		return false;
	}
	return true;
}


GameEntryBuilder::GameEntryBuilder(shared_ptr<DatabaseMaster> databases)
	:EntryBuilder(databases->GetEntryDatabase())
{
	mDatabases = databases;
	ClearBuild();
}

void GameEntryBuilder::ClearBuild()
{
	if (mCurrentGameEntry) { delete(mCurrentGameEntry); }
	mCurrentGameEntry = new GameEntry();
	mCurrentEntry = (Entry*)mCurrentGameEntry;

	mCurrentGameEntry->mGenres = GameGenres(mDatabases->GetGenreDatabase());
	mCurrentGameEntry->mTags = GameTags(mDatabases->GetTagDatabase());
}

void GameEntryBuilder::EditGameEntry(GameEntry& _gameEntry)
{
	ClearBuild();
	mCurrentGameEntry = new GameEntry(_gameEntry);
	mCurrentEntry = (Entry*)mCurrentGameEntry;
}

void GameEntryBuilder::EditGameEntry(ENTRYID _gameEntryId)
{
	if (mEntryDatabase->EntryExsists(_gameEntryId)) { std::cout << "Entry doesn't exsist to edit\n"; return; }
	return;
	//TODO: when implented entries to database rather than the summary
}

bool GameEntryBuilder::BuildGameEntry(shared_ptr<GameEntry>& gameEntry)
{
	//check all required entry fields are filled
	if (!RequiredFieldsCheck()) {
		std::cout << "Required entry are fields incomplete\n";
		return false;
	}

	//create a copy of the temp entry
	GameEntry outputEntry = GameEntry(*mCurrentGameEntry);

	//set id to 0, as it's not being added to the database right away 
	outputEntry.mId = 0;

	//set the returned out entry 
	gameEntry = make_shared<GameEntry>(outputEntry);

	return true;
}

bool GameEntryBuilder::BuildAndSaveGameEntry(shared_ptr<GameEntry>& gameEntry)
{
	//build the entry normally, check if valid
	if (BuildGameEntry(gameEntry)) { return false; }

	//get and set a unique id for this entry
	bool databaseCheck = mEntryDatabase->SetUniqueId(*gameEntry);

	//return if failed database checks
	if (!databaseCheck) { std::cout << "Entry failed database checks\n"; return false; }

	//add entry to database, and save to file
	mEntryDatabase->AddEntry(*gameEntry);
	mEntryDatabase->UpdateEntriesFile();

	return true;
}

void GameEntryBuilder::SetShortDescription(string _shortDescription)
{
	if (_shortDescription.empty() || _shortDescription.size() > GameEntry::SHORTDESCRIPTION_MAXLEN) {
		std::cout << "Invalid short description\n";
		return;
	}

	mCurrentGameEntry->SetShortDescription(_shortDescription);
}

void GameEntryBuilder::SetFullDescription(string _fullDescription)
{
	if (_fullDescription.empty() || _fullDescription.size() > GameEntry::FULLDESCRIPTION_MAXLEN) {
		std::cout << "Invalid full description\n";
		return;
	}

	mCurrentGameEntry->SetFullDescription(_fullDescription);
}

void GameEntryBuilder::SetTags(GameTags _tags)
{
	if (_tags.tagDatabase == nullptr) { std::cout << "Invalid tags\n"; return; }

	mCurrentGameEntry->mTags = _tags;
}

void GameEntryBuilder::SetTags(vector<string> _tags)
{
	if (_tags.size() == 0) { std::cout << "Invalid tags\n"; return; }

	mCurrentGameEntry->mTags = GameTags(mDatabases->GetTagDatabase());

	for (const string& tag : _tags) {
		AddTag(tag);
	}
}

void GameEntryBuilder::SetGenres(GameGenres _genres)
{
	if (_genres.genreDatabase == nullptr) { std::cout << "Invalid genres\n"; return; }

	mCurrentGameEntry->mGenres = _genres;
}

void GameEntryBuilder::SetGenres(vector<string> _genres)
{
	if (_genres.size() == 0) { std::cout << "Invalid genres\n"; return; }

	mCurrentGameEntry->mGenres = GameGenres(mDatabases->GetGenreDatabase());

	for (const string& genre : _genres) {
		AddGenre(genre);
	}
}

void GameEntryBuilder::SetRatings(GameRatings _ratings)
{
	mCurrentGameEntry->mRatings = _ratings;
}

void GameEntryBuilder::AddGenre(string _genre, bool addToDatabase)
{
	shared_ptr<GenreListDataBase> genreDatabase = mDatabases->GetGenreDatabase();

	if (genreDatabase->GenreExsists(_genre)) {
		mCurrentGameEntry->mGenres.AddGenre(genreDatabase->GetKey(_genre));
	}
	else if(addToDatabase) {
		genreDatabase->AddGenre(_genre);
		mCurrentGameEntry->mGenres.AddGenre(genreDatabase->GetKey(_genre));
	}
}

void GameEntryBuilder::AddTag(string _tag, bool addToDatabase)
{
	shared_ptr<TagListDataBase> tagDatabase = mDatabases->GetTagDatabase();

	if (tagDatabase->TagExsists(_tag)) {
		mCurrentGameEntry->mTags.AddTag(tagDatabase->GetKey(_tag));
	}
	else if (addToDatabase) {
		tagDatabase->AddTag(_tag);
		mCurrentGameEntry->mTags.AddTag(tagDatabase->GetKey(_tag));
	}
}

bool GameEntryBuilder::RequiredFieldsCheck()
{
	if (!EntryBuilder::RequiredFieldsCheck()) {
		std::cout << "Base entry requirements check failed\n";
		return false;
	}

	if (mCurrentGameEntry->mShortDescription.empty()) {
		std::cout << "Entry missing required field: " << "Short description\n";
		return false;
	}

	if (mCurrentGameEntry->mGenres.GetGenres().size() == 0) {
		std::cout << "Entry missing required field: " << "Genre\n";
		return false;
	}

	return true;
}
